// Main File: 		P3
// This File: 		mem.c
// Other Files:		mem.h
//			Makefile
//			tests/
// Semester:		CS 354 Spring 2017
// Author:		Matt Stout
// Email:		mcstout@wisc.edu
// CS Login:		stout
//
/////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include "mem.h"

/* Basic constants*/
#define WSIZE	4	/* Word and header/footer size (bytes)*/


/*
 * This structure serves as the header for each allocated and free block
 * It also serves as the footer for each free block
 * The blocks are ordered in the increasing order of addresses 
 */
typedef struct block_tag{

  int size_status;
  
 /*
  * Size of the block is always a multiple of 4
  * => last two bits are always zero - can be used to store other information
  *
  * LSB -> Least Significant Bit (Last Bit)
  * SLB -> Second Last Bit 
  * LSB = 0 => free block
  * LSB = 1 => allocated/busy block
  * SLB = 0 => previous block is free
  * SLB = 1 => previous block is allocated/busy
  * 
  * When used as the footer the last two bits should be zero
  */

 /*
  * Examples:
  * 
  * For a busy block with a payload of 24 bytes (i.e. 24 bytes data + an additional 4 bytes for header)
  * Header:
  * If the previous block is allocated, size_status should be set to 31
  * If the previous block is free, size_status should be set to 29
  * 
  * For a free block of size 28 bytes (including 4 bytes for header + 4 bytes for footer)
  * Header:
  * If the previous block is allocated, size_status should be set to 30
  * If the previous block is free, size_status should be set to 28
  * Footer:
  * size_status should be 28
  * 
  */

} block_tag;

/* Global variable - This will always point to the first block
 * i.e. the block with the lowest address */
block_tag *first_block = NULL;

/* Global variable - Total available memory */
int total_mem_size = 0;


/*
 * Function that searches the heap to find the best fitting block for the
 * requested block size to allocate
 * Arguments - size: the size of the requested block to allocate
 * Returns a pointer to the block to allocate to on success
 * Returns NULL if there is not block that fits the desired size
 */
static void *best_fit(int size) {
	// Best Fit Search
	int match = 0;
	int hold = 0;
	char *ptr = first_block;
	block_tag *current = (block_tag*)ptr;
	int c_size = current->size_status;
	while (current < (block_tag*)((char*)first_block + total_mem_size)) {
		if (c_size & 1) { //busy
			c_size -= 1;
		}
		else {
			if (c_size & 2) //prev is busy
				c_size -= 2;
			if (c_size == size) {
				ptr = current;
				match = 1;
			}
			else if (c_size > size && hold == 0) {
				ptr = current;
				hold = 1;
			}
			else {
				
			}
		}
		if (match == 1){
			hold = 1;
			break;
		}
		
		current += c_size;
	}

	if (hold == 1) 
		return ptr;
	else
		return NULL; //No fit for the requested block size
}


/*
 * Function that places the requested block at the beginning of the free block,
 * splitting only if the size of the remainder would equal or exceed the minimum
 * block size.
 * Arguments - ptr: a pointer to the location of the block to be allocated to
 * 	     - size: size of the block to be allocated
 */
static void place(void *ptr, size_t size) {
	int check_size = (block_tag*)ptr->size_status;
	int size_dif = check_size - size;
	
	/* Check if the block needs to be split
	 * Add appropriate headers and footers where necessary */
	if (size_dif >= (2*WSIZE)) {

		// Setting up the header of the allocated block
		block_tag *header = (block_tag*)ptr;
		header->size_status = size;
		// Marking allocated block as busy
		header->size_status += 1;
		// Marking previous block as busy
		header->size_status += 2;
		
		// Setting up the header for the block to be split
		block_tag *free_hdr = header + size;
		free_hdr->size_status = size_dif;
		// Marking previous block as busy
		free_hdr->size_status += 2;
		// Setting up the footer for the free split block
		block_tag *free_ftr = free_hdr + size_dif - 4;
		free_ftr->size_status = size_dif;
		// Marking previous block as busy
		free_ftr->size_status += 2;

	}
	else {  // Don't need to split into two blocks
		block_tag *header = (block_tag*)ptr;
		header->size_status = size;
		// Marking allocated block as busy
		header->size_status += 1;
		// Marking preious block as busy
		header->size_status += 2;
	}
}

/*
 * Fuction for allocating 'size' bytes
 * Returns the address of the payload in the allocated block on success
 * Returns NULL on failure
 * Here is what this function should accomplish
 * - If size is less than equal to 0 - Return NULL
 * - Round up size to a multiple of 4
 * - Traverse the list of blocks and allocate the best free block which can accommodate the requested size 
 * - Also, when allocating a block - split it into two blocks when possible
 * Tips: be careful with pointer arithmetic
 */
void *Mem_Alloc(int size) {
	int blk_size;
	char *ptr;

	// Ignore requests with incorrect size
	if (size <= 0)
		return NULL;

	// Adjust block size to meet alignment requirements
	if (size <= WSIZE)
		blk_size = 2*WSIZE;
	else
		blk_size = WSIZE * ((size + WSIZE + (WSIZE-1)) / WSIZE);
	
	// Search the free list to find the  best fit block to allocate to
	if ((ptr = best_fit(blk_size)) != NULL) {
		place(ptr, blk_size);
		ptr += WSIZE;
		return ptr;
	}
	
	// No block found that fits the requested block size
	return NULL;
	
}


/*
 * Function to coalesce adjecent free blocks
 * Argument - ptr: a pointer to the current block, which will be check
 *		to see if it can be coalesced with adjacent free blocks
 * Returns pointer to the entire free block
 */
static void *coalesce(void *ptr) {
	block_tag *current = (block_tag*)ptr;
	int size = current->size_status;
	int prev_alloc = 0; // 0 = free, 1 = busy
	if (size & 2) //check if previous block is free
		prev_alloc = 1;
	block_tag *next = (block_tag*)((char*)ptr + size);
	int next_alloc = 0; // 0 = free, 1 = busy
	int next_size = next->size_status;
	if (next_size & 2)
		next_alloc =1;
	
	
	// Case 1: freed block doesn't get coalesced with adjacent blocks (busy)
	if ((prev_alloc == 1) && (next_alloc == 1)) {
		return ptr;
	}
	// Case 2: freed block gets coalesced with the next block which is also free
	else if ((prev_alloc == 1) && (next_alloc == 0)) {
		size += next_size;
		current->size_status = size;
		block_tag *footer = (block_tag*)((char*)ptr + size - WSIZE);
		footer->size_status = size;
	}
	// Case 3: freed block gets coalesced with the previous block which is also free
	else if (prev_alloc == 0) { //prev block is free
		block_tag *prev_footer = (block_tag*)((char*)ptr - WSIZE);
		int prev_size = prev_footer->size_status - 2;
		size += prev_size;
		ptr -= prev_size;
		current = (block_tag*)((char*)ptr);
		current->size_status = size;
		if (next_alloc == 1) {
			//Mark previous as busy
			current->size_status += 2;
			block_tag *footer = (block_tag*)((char*)ptr + (size-2) - WSIZE);
			footer->size_status = size;
		}
	// Case 4: freed block gets coalesced with previous and next blocks (both are free)
		else { //next_alloc = 0 (next block is free)
			size += next_size;
			current->size_status = size;
			block_tag *footer = (block_tag*)((char*)ptr + size - WSIZE);
			// Mark prev as busy
			footer->size_status = size + 2;
			current->size_status += 2;
		}
	}
	return ptr;
}

/*
 * Function for freeing up a previously allocated block
 * Argument - ptr: Address of the payload of the allocated block to be freed up
 * Returns 0 on success
 * Returns -1 on failure
 * Here is what this function should accomplish
 * - Return -1 if ptr is NULL
 * - Return -1 if ptr is not within the range of memory allocated by Mem_Init()
 * - Return -1 if ptr is not 4 byte aligned
 * - Mark the block as free
 * - Coalesce if one or both of the immediate neighbors are free
 */
int Mem_Free(void *ptr) {
	// Check if ptr is NULL
	if (ptr == NULL)
		return -1;

	// Check if ptr is within the range of memory allocated by Mem_Init()
	if (ptr < (block_tag*)((char*)first_block) || ptr > (block_tag*)((char*)first_block + total_mem_size))
		return -1;

	// Check if ptr is 4 byte aligned
	int busy = 0;
	block_tag *header = (block_tag*)((char*)ptr - WSIZE);
	int size = header->size_status;
	// Block should be marked busy, so adjust size
	// Simultaneously "mark as free" if block is successfully 4 byte aligned
	size -= 1;
	// Check if previous block is busy, and adjust size
	if (size & 2) { //prev block is busy
		size -= 2;
		busy = 1;
	}
	// If size is not divisible by 4, return NULL due to incorrect alignment
	if ((size % 4) != 0)
		return -1;
	
	// Mark the block as free
	if (busy == 1) //fix size to indicate prev block status if it was busy
		size += 2;
	header->size_status = size;
	
	// Add footer to the freed block
	block_tag *footer = header + size - WSIZE;
	footer->size_status = size;
	
	// Coaliesce if one or both of the immediate neighbors are free
	ptr = ptr - WSIZE; //point to the header of the block
	coalesce(ptr);
	
	return 0;	
}


/*
 * Function used to initialize the memory allocator
 * Not intended to be called more than once by a program
 * Argument - sizeOfRegion: Specifies the size of the chunk which needs to be allocated
 * Returns 0 on success and -1 on failure 
 */
int Mem_Init(int sizeOfRegion){
  int pagesize;
  int padsize;
  int fd;
  int alloc_size;
  void* space_ptr;
  static int allocated_once = 0;
  
  if(0 != allocated_once){
    fprintf(stderr,"Error:mem.c: Mem_Init has allocated space during a previous call\n");
    return -1;
  }
  if(sizeOfRegion <= 0){
    fprintf(stderr,"Error:mem.c: Requested block size is not positive\n");
    return -1;
  }

  // Get the pagesize
  pagesize = getpagesize();

  // Calculate padsize as the padding required to round up sizeOfRegion to a multiple of pagesize
  padsize = sizeOfRegion % pagesize;
  padsize = (pagesize - padsize) % pagesize;

  alloc_size = sizeOfRegion + padsize;

  // Using mmap to allocate memory
  fd = open("/dev/zero", O_RDWR);
  if(-1 == fd){
    fprintf(stderr,"Error:mem.c: Cannot open /dev/zero\n");
    return -1;
  }
  space_ptr = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
  if (MAP_FAILED == space_ptr){
    fprintf(stderr,"Error:mem.c: mmap cannot allocate space\n");
    allocated_once = 0;
    return -1;
  }
  
  allocated_once = 1;
  
  // Intialising total available memory size
  total_mem_size = alloc_size;

  // To begin with there is only one big free block
  first_block = (block_tag*) space_ptr;
  
  // Setting up the header
  first_block->size_status = alloc_size;
  // Marking the previous block as busy
  first_block->size_status += 2;

  // Setting up the footer
  block_tag *footer = (block_tag*)((char*)first_block + alloc_size - 4);
  footer->size_status = alloc_size;
  
  return 0;
}

/* 
 * Function to be used for debugging 
 * Prints out a list of all the blocks along with the following information for each block 
 * No.      : serial number of the block 
 * Status   : free/busy 
 * Prev     : status of previous block free/busy
 * t_Begin  : address of the first byte in the block (this is where the header starts) 
 * t_End    : address of the last byte in the block 
 * t_Size   : size of the block (as stored in the block header)(including the header/footer)
 */ 
void Mem_Dump() {
  int counter;
  char status[5];
  char p_status[5];
  char *t_begin = NULL;
  char *t_end = NULL;
  int t_size;

  block_tag *current = first_block;
  counter = 1;

  int busy_size = 0;
  int free_size = 0;
  int is_busy = -1;

  fprintf(stdout,"************************************Block list***********************************\n");
  fprintf(stdout,"No.\tStatus\tPrev\tt_Begin\t\tt_End\t\tt_Size\n");
  fprintf(stdout,"---------------------------------------------------------------------------------\n");
  
  while(current < (block_tag*)((char*)first_block + total_mem_size)){

    t_begin = (char*)current;
    
    t_size = current->size_status;
    
    if(t_size & 1){
      // LSB = 1 => busy block
      strcpy(status,"Busy");
      is_busy = 1;
      t_size = t_size - 1;
    }
    else{
      strcpy(status,"Free");
      is_busy = 0;
    }

    if(t_size & 2){
      strcpy(p_status,"Busy");
      t_size = t_size - 2;
    }
    else strcpy(p_status,"Free");

    if (is_busy) busy_size += t_size;
    else free_size += t_size;

    t_end = t_begin + t_size - 1;
    
    fprintf(stdout,"%d\t%s\t%s\t0x%08lx\t0x%08lx\t%d\n",counter,status,p_status,
                    (unsigned long int)t_begin,(unsigned long int)t_end,t_size);
    
    current = (block_tag*)((char*)current + t_size);
    counter = counter + 1;
  }
  fprintf(stdout,"---------------------------------------------------------------------------------\n");
  fprintf(stdout,"*********************************************************************************\n");

  fprintf(stdout,"Total busy size = %d\n",busy_size);
  fprintf(stdout,"Total free size = %d\n",free_size);
  fprintf(stdout,"Total size = %d\n",busy_size+free_size);
  fprintf(stdout,"*********************************************************************************\n");
  fflush(stdout);
  return;
}

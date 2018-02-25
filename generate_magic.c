////////////////////////////////////////////////////////////////////////////////
// Main File:        P2
// This File:        generate_magic.c
// Other Files:      verify_magic.c
// Semester:         CS 354 Spring 2017
//
// Author:           Matt Stout
// Email:            mcstout@wisc.edu
// CS Login:         stout
//
/////////// IF PAIR PROGRAMMING IS ALLOWED, COMPLETE THIS SECTION //////////////
//
// Pair Partner:     (name of your pair programming partner)
// Email:            (email address of your programming partner)
// CS Login:         (partner's CS login name)
//
/////////////////////////// OTHER SOURCES OF HELP //////////////////////////////
//                   fully acknowledge and credit all sources of help,
//                   other than Instructors and TAs.
//
// Persons:          Identify persons by name, relationship to you, and email.
//                   Describe in detail the the ideas and help they provided.
//
// Online sources:   avoid web searches to solve your problems, but if you do
//                   search, be sure to include Web URLs and description of 
//                   of any information you find.
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

// Structure representing Square
// size: dimension(number of rows/columns) of the square
// array: 2D array of integers
typedef struct _Square {
	int size;
	int **array;
} Square;

int get_square_size();
Square * generate_magic(int size);
void write_to_file(Square * square, char *filename);

int main(int argc, char *argv[])
{
	// Check input arguments to get filename
	if(argv == NULL) {
		printf("No file name entered to write to.");
		exit(1);
	}

	// Get size from user
	int size = get_square_size();
	if (size < 3 || (size%2) == 0) {
		printf("Size must be an odd number >= 3");
		exit(1);
	}

	// Generate the magic square
	Square square = generate_magic(size);

	// Write the square to the output file
	write_to_file(square, *argv);

	// Free the space in Square's array
	for (int i = 0; i < square.size; i++) free(square.*array);
	free(square.array);

	return 0;
}

/* get_square_size prompts the user for the magic square size
 * checks if it is an odd number >= 3 and returns the number
 */
int get_square_size()
{
	int size = 0;
	fprintf("Enter size of magic square, must be odd\n");
	size = getc(stdin);
	return size;
}

/* generate_magic constructs a magic square of size n
 * using the Siamese algorithm and returns the Square struct
 */
Square * generate_magic(int n)
{
	// Initialize a Square struct
	Square square;
	square.size = n
	square.array = malloc(sizeof(int*)*n);
	for (int i = 0; i < square.size; i++) {
		square.*array = malloc(sizeof(int)*n);
	}
	
	// Assign values to the struct array to create magic square
	int i = 0; // Keep track of the row, startin in the top
	// Keep track of the column, starting in the middle
	int j = (square.size / 2) + 1; 
	for (int count = 1; count <= (square.size*square.size); count++) {
		*(*(square.array+i)+j) = count;
		
		// Get the next position, checking if the position
		// extends beyond the square
		if ((i--1) < 0) {
			i = square.size-1;
		} else {
			i--;
		}
		if (j++ == square.size) {
			j = 0;
		} else {
			j++;
		}

		// If the next position is full, move position down 1 row
		if ( *(*(square.array+i)+j) != NULL) {
			i++;
		}
	}

	return square;
}

/* write_to_file opens up a new file(or overwrites the existing file)
 * and writes out the square in the format expected by verify_magic.c
 */
void write_to_file(Square * square, char *filename)
{
	// Open the file to write to, or create a new file
	// if file does not already exist
	FILE *fp;
	fp = fopen(*filename, "w");

	// Write the square to the file in the expected format
	fprintf(fp, "%d\n", square.size);
	for (int i = 0; i < square.size; i++) {
		for (int j = 0; j < square.size-1; j++) {
			fprintf(fp, "%d,", *(*(square.array+i)+j));
		}
		j++;
		fprintf(fp, "%d\n", *(*(square.array+i)+j));
	}
	
	fclose(fp);
}


















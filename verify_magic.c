////////////////////////////////////////////////////////////////////////////////
// Main File:        P2
// This File:        verify_magic.c
// Other Files:      generate_magic.c
// Semester:         CS 354 Spring 2017
//
// Author:           Matt Stout
// Email:            mcstout@wisc.edu
// CS Login:         stout
//
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure representing Square
// size: dimension(number of rows/columns) of the square
// array: 2D array of integers
typedef struct Square {
	int size;
	int **array;
} Square;

Square * construct_square(char *filename);
int verify_magic(Square * square);

int main(int argc, char *argv[])
{
	// Check input arguments to get filename
	if (argv == NUUL) {
		printf("No file was entered.");
		exit(1);
	}

	// Construct square
	construct_square(argv);

	// Verify if it's a magic square and print true or false
	if (verify_magic == 1) {
		printf("true");
	}
	else {
		printf("false");
	}
	
	// Free the space for the Square's **array
	for (int i = 0; i < square.size; i++) free(square.*array);
	free(square.array);
	
	return 0;
}

/* construct_square reads the input file to initialize a square struct
 * from the contents of the file and returns the square.
 * The format of the file is defined in the assignment specifications
 */
Square * construct_square(char *filename)
{

	// Open and read the file
	FILE *fp;
	fp = fopen(filename, "r");
	if (fp == NULL) {
		printf(stderr, "Can't open file for reading.");
		exit(1);
	}

	// Read the first line to get the square size
	char *c;
	c = fgets(*c, 100, *fp);
	int n = getc(*c);
	int **a = malloc(sizeof(int)*n);
	for(int i = 0; i < n; i++) {
		*m = malloc(sizeof(int)*n);
	}

	// Initialize a new Square struct of that size
	Square square;
	square.size = n;
	square.array = a;

	// Read the rest of the file to fill up the square array
	char s;
	for(int i = 0; i < n && (c = fgets(*c, n*3, *fp)) != EOF; i++) {
		c = fgets(*c, n*3, *fp);
		for(int j = 0; j < n && getc(c) != NULL; j++) {
			while( (s = getc(*c)) == ",") {
				s = getc(*c);
			}
			int holdVal = getc(*c);
			*(*(a+i)+j) = holdVal;
		}
	}

	fclose(fp);

	return square;
}

/* verify_magic verifies if the square is a magic square
 * 
 * returns 1(true) or 0(false)
 */
int verify_magic(Square * square)
{
	// Check all rows sum to same number
	int sumRow = 0;
	int sumPrevRow = 0;
	for(int i = 0; i < square.size; i++) {
		for(int j = 0; j < square.size; j++) {
			sumRow += *(*(square.array+i)+j);
		}
		if (sumPrevRow == 0 || sumRow == sumPrevRow) {
			sumPrevRow = sumRow;
			sumRow = 0;
		}
		else {
			return 0;
		}
	}

	// Check all cols sum to same number
	int sumCol = 0;
	int sumPrevCol = 0;
	for(int j = 0; j < square.size; j++) {
		for (int i = 0; i < square.size; i++) {
			sumCol += *(*(square.array+j)+i);
		}
		if (sumPrevCol == 0 || sumCol == sumPrevCol) {
			sumPrevCol = sumCol;
			sumCol = 0;
		}
		else {
			return 0;
		}
	}
	if (sumPrevCol != sumPrevRow) {
		return 1:
	}

	// Check main diagonal
	int diagSum = 0;
	for (int i = 0; i < square.size; i++) {
		diagSum += *(*(square.array+i)+i);
	}
	if (diagSum != sumPrevCol) {
		return 0;
	}

	// Check secondary diagonal
	diagSum = 0;
	for (int i = n-1; i >= 0; i--) {
		diagSum += *(*(square.array+n)+n);
	}
	if (diagSum != sumPrevCol) {
		return 0;
	}

	// If none of the checks above failed, return true
	return 1;
}	

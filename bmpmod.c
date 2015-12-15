#include <stdio.h>
#include "bmp.h"
#include "filter.h"

/*
 * The program "bmpmod" takes a bitmap image as an input, and applies a number of 
 * user-defined filters to the image. Finally, the image is saved in a user-defined bitmap output file.
 * 
 * This file, "bmpmod.c" is responsible for handling input from the user and passing the correct parameters 
 * to it's supporting files and functions.
 * 
 * Command Syntax: ./bmpmod <filter-1>...<filter-n> <inputFile.bmp> <outputFile.bmp>
 * 
 * @author Andrew Hoffman
 * @version 08/18/2015
*/
int main( int argc, char* argv[] ) {
	int i        = 0; /* For iterating through filters. */
	int nFilters = 0; /* For holding the number of filters passed in via the user. */

	/* BMP Struct is defined inside of "bmp.h". */
	BMP *bmp;         /* The original bitmap image. */
	BMP *bmpcpy;      /* A copy of the original bitmap image.*/

	/* A filter function. */
	void (*filterFunction)(BMP*, BMP*, int, int);
	/* An array of filter functions. */
	void (*filterFunctionSet[64])(BMP*, BMP*, int, int);

	/* 
	 * Iterate through all filters passed in via the user.
	 * Throw an error if the filter does not currently exist.
	 * Otherwise, add the filter to filterFunctionSet.
	*/
	for (i = 1; i < argc - 2 && nFilters < 64 ; i++) {
		/* Check to ensure the filter exists. */
		if (!(filterFunction = parse_filter(argv[i]))) {
			/* Return an error and exit the program if a bad filter is provided. */
			fprintf( stderr, "Unrecognized filter %s\n", argv[i]);
			return (0);
		}
		/* If the filter exists, add it to the set of filters to be applied to this bitmap image. */
		filterFunctionSet[nFilters++] = filterFunction;
	}

	/* Read in and store the input file. */
	bmp = BMP_read( argv[ argc - 2 ] );
	/* Create a copy of the input file. */
	bmpcpy = BMP_clone(bmp);

	/*
	 * Iterate through all filters specified by the user. 
	 * Apply each filter to the "bmpcpy" bitmap image.
	*/
	for (i = 0; i < nFilters; i++) {
		applyFilter(bmpcpy, bmp, filterFunctionSet[i]);
		/* After applying each filter to "bmpcpy", copy the data back to the original bitmap image("bmp"). */
		BMP_copyData(bmp, bmpcpy); 
	}
	
	/* Write the changes to disk.
	 * Pass through the output file location and name specified by the user. 
	*/
	BMP_write( bmp, argv[ argc - 1 ] );
	/* Eliminate the local bitmap "bmp", freeing memory. */
	BMP_destroy( bmp);
	/* Eliminate the local bitmap copy "bmpcpy", freeing memory. */
	BMP_destroy( bmpcpy);

	/* Return 1 if the program executed successfully. */
	return (1);
}


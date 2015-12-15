#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "bmp.h"

/* 
 * First, we must read in the bitmap image from the filesystem.
*/
BMP* BMP_read(char *filename) {
	/* Create 16 & 32 bit unsigned integers to represent the dimensions of the input file. */
	uint32_t *dataOffsetPtr, *heightPtr, *widthPtr, *fileSizePtr, *dataSizePtr;
	uint16_t *depthPtr;

	/* Allocate memory for a BMP object. */
	BMP *bmp = malloc(sizeof(BMP));

	/* Open the user-specified file and prepare it for reading. */
	FILE *fp = fopen(filename, "r");

	/* Read headers into the BMP struct. */
	fread(bmp->header, 14, 1, fp);
	fread(bmp->dibheader, 40, 1, fp);

	/* Initialize offset pointer. */
	dataOffsetPtr = (uint32_t*) (bmp->header + 10);

	if ((bmp->other1Size = (*dataOffsetPtr) - 54)) {
		bmp->other1 = malloc(bmp->other1Size);
		fread(bmp->other1, bmp->other1Size, 1, fp);
	}

	/* Collect dimensions from the input file. */
	widthPtr      =	(uint32_t*)(bmp->dibheader + 4);
	heightPtr     = (uint32_t*)	(bmp->dibheader + 8);
	depthPtr      = (uint16_t*) (bmp->dibheader + 14);
	dataSizePtr   =	(uint32_t*)(bmp->dibheader + 20);
	/* Read dimensions into the BMP struct. */
	bmp->width    = *widthPtr;
	bmp->height   = *heightPtr;
	bmp->depth    = *depthPtr;
	bmp->dataSize = *dataSizePtr;

	/* Allocate memory for the data field. */
	bmp->data = malloc(bmp->dataSize);
	fread(bmp->data, 1, bmp->dataSize, fp);
	/* Continue reading input file. */
	fileSizePtr = (uint32_t*)(bmp->header + 2);;
	if ((bmp->other2Size = *fileSizePtr - ftell(fp))) {
		fread(bmp->other2, 1, bmp->other2Size, fp);
	}

	/* Close input file to preserve memory. */
	fclose(fp);

	/* Return the struct containing the input file's data. */
	return (bmp);
}

/*
 * This function allows us to write to the user-specified output file.
*/
void BMP_write(BMP *bmp, char *filename) {
	/* Open the output file, and prepare for writing. */
	FILE *fp = fopen(filename, "w");
	/* Write headers to output file. */
	fwrite(bmp->header, 14, 1, fp);
	fwrite(bmp->dibheader, 40, 1, fp);
	/* Write data to output file, provided it exists. */
	if (bmp->other1Size) fwrite(bmp->other1, bmp->other1Size, 1, fp);
	if (bmp->dataSize) fwrite(bmp->data, bmp->dataSize, 1, fp);
	if (bmp->other2Size) fwrite(bmp->other2, bmp->other2Size, 1, fp);
	/* Close the output file to preserve memory. */
	fclose(fp);
}

/*
 * This function destroys a bitmap "BMP" struct in order to free memory for other applications.
*/
void BMP_destroy(BMP *bmp) {
	/* Determine what memory to free for this bitmap image. Free the bitmap's data. */
	if (bmp->dataSize)		free(bmp->data);
	if (bmp->other1Size)	free(bmp->other1);
	if (bmp->other2Size)	free(bmp->other2);
	/* Free the struct's memory. */
	free(bmp);
}

/* 
 * This function produces a clone of an existing bitmap "BMP" struct.
*/
BMP* BMP_clone (BMP *src) {
	/* 
	 * Create a new BMP struct.
	 * Allocate memory to it equal to the memory required by the original struct.
	*/
	BMP *bmpcpy = malloc(sizeof(BMP)); 

	/* 
	 * Point the copy to the original. 
	 * This produces a shallow copy, where "bmpcpy" is vulnerable to modification via other functions acting
	 * on "src".
	*/
	*bmpcpy = *src;	

	/*
	 * Examine each block. If the sizes are not equal to zero, allocate and equal amount of memory 
	 * in the "bmpcpy" and than move the original data to "bmpcpy". 
	 *
	 * This gives us a deeper copy which is less vulnerable to interference and issues.
	*/
	if (src->other1Size) {
		bmpcpy->other1 = malloc(src->other1Size);
		memmove(bmpcpy->other1, src->other1, src->other1Size);
	}
	if (src->other2Size) {
		bmpcpy->other2 = malloc(src->other2Size);
		memmove(bmpcpy->other2, src->other2, src->other2Size);
	}
	if (src->dataSize) {
		bmpcpy->data = malloc(src->dataSize);
		memmove(bmpcpy->data, src->data, src->dataSize);
	}

	/* Return the copied bitmap. */
	return (bmpcpy);
}

/*
 * This function copies any modifications on the temporary "bmpcpy" to the original bitmap.
*/
void BMP_copyData (BMP *dest, BMP *src) {
	/* Copy changes over from dest to src. */
	memmove(dest->data, src->data, src->dataSize);
}

/* 
 * This function analyzes the provided bitmap, and determines it's depth. It also manages pixel pointers.
*/
void BMP_getPixel(BMP *bmp, int x, int y, unsigned char *r, unsigned char *g, unsigned char *b) {

	/* Determine the number of bytes in each row of data. */
	int bytes_per_row = bmp->dataSize / bmp->height;

	/* Determine the size of each row, given the number of bytes_per_row. */
	int index = y * bytes_per_row + x * bmp->depth / 8;

	/* Provided the bitmap has a depth greater than 16 bits... */
	if (bmp->depth > 16) {
		/* First, grab the pixel pointer via casting as a unsigned 32 bit integer pointer. */
		uint32_t *pointer = (uint32_t *) (bmp->data + index);
		/* Next, grab the pixel data from the pointer using the * operator. */
		uint32_t pixel = *pointer;
		*r = pixel >> 16; /* 00000000 00000000 bbbbbbbb RRRRRRRR */
		*g = pixel >> 8;  /* 00000000 AAAAAAAA RRRRRRRR GGGGGGGG */
		*b = pixel;       /* AAAAAAAA RRRRRRRR GGGGGGGG BBBBBBBB */
	}

	/* Assuming this bitmap's depth is equal to 16 bits... */
	else if (bmp->depth == 16) {
		/* Grab the pixel pointer via casting as an unsigned 32 bit integer. */
		uint16_t *pointer = (uint16_t *) (bmp->data + index);
		/* Grab the data from the pointer using the * operator. */
		uint16_t pixel = *pointer;
		/* Utilize a 0x1F mask. This lets us define the bits we wish to keep. */
		char mask = 0x1F;
		*b = (char)(pixel & mask);        /* Note: & represents a binary AND operation. */
		*g = (char)((pixel >> 5) & mask); /* Note: >> represents a right-wards binary shift. */
		*r = (char)((pixel >> 10) & mask);
	}
}

/* 
 * This function allows us to set the value of a pixel in our bitmap.
*/
void BMP_setPixel(BMP *bmp, int x, int y, unsigned char r, unsigned char g, unsigned char b) {
	/* First, determine bytes per row. */
	int bytes_per_row = bmp->dataSize / bmp->height;
	/* Next, find the size of each row. */
	int index = y * bytes_per_row + x * bmp->depth / 8;

	/* If the bitmap's depth is greater than 16 bits... */
	if (bmp->depth > 16) {
		/* Grab the pixel pointer and cast it as a 32 bit unsigned int pointer. */
		uint32_t *pixel32ptr = (uint32_t*)((bmp->data) + index);
		/* Grab the pixel data from the pixel32ptr using the * operator. */
		uint32_t pixel32 = *pixel32ptr;

		/* Find RGB values. */
		pixel32 &= 0xFF000000;
		uint32_t r32 = r;
		uint32_t g32 = g;
		uint32_t b32 = b;

		pixel32 |= (r32	<< 16) | (g32 << 8) | b32;

		/* Set pixel pointer to the pixel32 data. */
		*pixel32ptr = pixel32;
	}

	/* If the bitmap's depth is equal to 16 bits... */
	else if (bmp->depth == 16) {
		/* Grab the pixel pointer and cast it as an unsigned 16 bit int pointer. */
		uint16_t *pixel16ptr = (uint16_t*)((bmp->data) + index);
		uint16_t pixel16     = 0;

		/* Find RGB values. */
		uint16_t r16         = r;
		uint16_t g16         = g;
		uint16_t b16         = b;

		pixel16 |= (r16 << 10) | (g16 << 5) | b16;

		/* Set pixel pointer to the pixel32 data. */
		*pixel16ptr = pixel16;
	}
}

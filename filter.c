#include "bmp.h"
#include "filter.h"

/*
 * This function is responsible for determining the correct filter to run on the input image.
*/
void (*parse_filter(char *string)) (BMP*, BMP*, int, int) {

	/* Evaluate availible functions against the input parameters. */
	if (strcmp(string, "blur")               == 0) {
		return (blurFunction);
	} else if (strcmp(string, "invertColor") == 0) {
		return (invertColorFunction);
	} else if (strcmp(string, "flipH")       == 0) {
		return (flipHFunction);
	} else if (strcmp(string, "flipV")       == 0) {
		return (flipVFunction);
	} else if (strcmp(string, "sharpen")     == 0) {
		return (sharpenFunction);
	} else {
		fprintf(stderr, "Internal Error. Filter invalid.");
		return (0);
	}
}

/*
 * The "applyFilter" function iterates over all pixels in the provided BMP and calls filter functions.
*/
void applyFilter(BMP *dest, BMP *src, void filterFunction(BMP*, BMP*, int, int)) {
	int x, y;
	/* Iterate accross all pixels. */
	for (x = 0; x < src->width; x++) {
		for (y = 0; y < src->height; y++) {
			filterFunction(dest, src, x, y);
		}
	}
}

/* 
 * This function applies an average to nearby pixels resulting in a "blur" effect.
*/
void blurFunction(BMP *dest, BMP *src, int x, int y) {
	/* The weights array will be applied to RGB sums. */
	int weights[3][3] = {{1, 2, 1},
		{2, 4, 2},
		{1, 2, 1}
	};

	/* Instantiate variables, and iterate accross pixels just like the sharpen function does. */
	int maxRow = src->height - 1;
	int maxCol = src->width - 1;
	int maxRGBValue = (src->depth > 16) ? 255 : 31;
	int i, j;
	int rsum = 0;
	int gsum = 0;
	int bsum = 0;

	/* 
	 * My understanding is that we are iterating across all pixels, and sampling R G and B values.
	 * Once we have an "average" for R, G & B we can set all the pixels equal to the average (or near-average).
	*/
	for (i = -1; i < 2; i++) {
		int xij = x + i;
		if (xij < 0) xij = 0;
		if (xij > maxCol) xij = maxCol;
		for (j = -1; j < 2; j++) {
			unsigned char r, g, b;
			int yij = y + j;
			if (yij < 0) yij = 0;
			if (yij > maxRow)yij = maxRow;
			BMP_getPixel(src, xij, yij, &r, &g, &b);
			rsum += r * weights[i + 1][j + 1];
			gsum += g * weights[i + 1][j + 1];
			bsum += b * weights[i + 1][j + 1];
		}
	}

	/* Normalize by dividing sums by 16. */
	rsum = rsum / 16;
	gsum = gsum / 16;
	bsum = bsum / 16;

	/* Prevent negative RGB values in the RGB averages. */
	if (rsum < 0) rsum = 0;
	if (rsum > maxRGBValue) rsum = maxRGBValue;
	if (gsum < 0) gsum = 0;
	if (gsum > maxRGBValue) gsum = maxRGBValue;
	if (bsum < 0) bsum = 0;
	if (bsum > maxRGBValue) bsum = maxRGBValue;

	BMP_setPixel(dest, x, y, rsum, gsum, bsum);
}

/*
 * The function "sharpenFuntion" sharpens all of the pixels in bitmap by increasing the gap in their RGB values. 
*/
void sharpenFunction(BMP *dest, BMP *src, int x, int y) {
	int weights[3][3] = {{ -1, -1, -1},
		{ -1, 9, -1},
		{ -1, -1, -1}
	};
	int maxRow = src->height - 1;
	int maxCol = src->width - 1;
	int maxRGBValue = (src->depth > 16) ? 255 : 31;
	int i, j;
	int rsum = 0;
	int gsum = 0;
	int bsum = 0;

	for (i = -1; i < 2; i++) {
		int xij = x + i;
		if (xij < 0) xij = 0;
		if (xij > maxCol) xij = maxCol;
		for (j = -1; j < 2; j++) {
			unsigned char r, g, b;
			int yij = y + j;
			if (yij < 0) yij = 0;
			if (yij > maxRow)yij = maxRow;
			BMP_getPixel(src, xij, yij, &r, &g, &b);
			rsum += r * weights[i + 1][j + 1];
			gsum += g * weights[i + 1][j + 1];
			bsum += b * weights[i + 1][j + 1];
		}
	}
	if (rsum < 0) rsum = 0;
	if (rsum > maxRGBValue) rsum = maxRGBValue;
	if (gsum < 0) gsum = 0;
	if (gsum > maxRGBValue) gsum = maxRGBValue;
	if (bsum < 0) bsum = 0;
	if (bsum > maxRGBValue) bsum = maxRGBValue;

	BMP_setPixel(dest, x, y, rsum, gsum, bsum);
}

/*
 * The "invertColorFunction" reads in RGB values and "inverts" the colors by subtracting them from the max RGB value.
*/
void invertColorFunction(BMP *dest, BMP *src, int x, int y) {
	/* Find max RGB value. */
	int maxRGBValue = (src->depth > 16) ? 255 : 31;
	/* Create RGB chars. */
	unsigned char r, g, b;
	/* Get the pixel(source: applyFilter()) */
	BMP_getPixel(src, x, y, &r, &g, &b);
	/* Set the pixel with inverted RGB value. */
	BMP_setPixel(dest, x, y, maxRGBValue - r, maxRGBValue - g, maxRGBValue - b);
}

/* 
 * The "flipVFunction" flips the image on the vertical axis. This is done by reading in a pixel at the same x point, but at the opposite end
 * of y - and than saving that pixel to (x, y).
*/
void flipVFunction(BMP *dest, BMP *src, int x, int y) {
	/* Find maxRow value just like in the sharpen/blur functions. */
	int maxRow = src->height - 1;
	/* Create RGB chars. */
	unsigned char r, g, b;
	/* Get the pixel opposite of (x, y) on the y-axis. */
	BMP_getPixel(src, x, maxRow - y, &r, &g, &b);
	/* Set the pixel as (x, y) */
	BMP_setPixel(dest, x, y, r, g, b);
}

/* 
 * The "flipHFunction" flips the image on the horizontal axis. It is done the same way as "flipVFunction", but relies on max columns
 * rather than max rows to find the opposing pixel.
*/
void flipHFunction(BMP *dest, BMP *src, int x, int y) {
	/* Find maxCol value just like in the sharpen/blur functions. */
	int maxCol = src->width - 1;
	/* Create RGB Chars. */
	unsigned char r, g, b;
	/* Get the pixel opposite of (x, y) on the x axis. */
	BMP_getPixel(src, maxCol - x, y, &r, &g, &b);
	/* Set the pixel as (x, y) */
	BMP_setPixel(dest, x, y, r, g, b);
}

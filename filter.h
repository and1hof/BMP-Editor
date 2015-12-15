#ifndef _BMP_H_
#define _BMP_H_
#include "bmp.h"
#endif
#include <stdlib.h>
void (*parse_filter(char *string)) (BMP*, BMP*, int, int); /*returns a function pointer*/

void applyFilter(BMP *dest, BMP *src, void filterFunction(BMP*, BMP*, int, int)); /*applies the filter functions to the image*/

/*filter functions*/
void blurFunction	(BMP *dest, BMP *src, int x, int y);
void sharpenFunction	(BMP *dest, BMP *src, int x, int y);
void invertColorFunction(BMP *dest, BMP *src, int x, int y);
void flipVFunction(BMP *dest, BMP *src, int x, int y);
void flipHFunction(BMP *dest, BMP *src, int x, int y);

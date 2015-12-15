#ifndef _BMP_H_
#define _BMP_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* Bitmap header 14 bytes */
/*
	uint16_t		Magic;				     0  Magic identifier: "BM"
	uint32_t		FileSize;			   2  Size of the BMP file in bytes
	uint16_t		Reserved1;		   6  Reserved
	uint16_t		Reserved2;		   8  Reserved
	uint32_t		DataOffset;	   10 Offset of image data relative to the file's start
*/

/* dib header at least 40 bytes */
/*
	uint32_t		HeaderSize;		        0 Size of the header in bytes
	uint32_t		Width;				           4 Bitmap's width
	uint32_t		Height;				          8 Bitmap's height
	uint16_t		Planes;				          12 Number of color planes in the bitmap
	uint16_t		BitsPerPixel;		      14 Number of bits per pixel
	uint32_t		CompressionType;	    16 Compression type
	uint32_t		ImageDataSize;		     20 Size of uncompressed image's data
	uint32_t		HPixelsPerMeter;	    24 Horizontal resolution (pixels per meter)
	uint32_t		VPixelsPerMeter;	    28 Vertical resolution (pixels per meter)
	uint32_t		ColorsUsed;			       32 Number of color indexes in the color table that are actually used by the bitmap
	uint32_t		ColorsRequired;		    36 Number of color indexes that are required for displaying the bitmap
*/
typedef struct {
	unsigned char header[14];
	unsigned char dibheader[40];
	unsigned char *other1;
	unsigned char *data;
	unsigned char *other2;
	uint32_t height;
	uint32_t width;
	uint16_t depth;
	uint32_t dataSize;
	uint32_t other1Size;
	uint32_t other2Size;
} BMP;

BMP* BMP_read(char *filename);
void BMP_write(BMP *bmp, char *filename);
void BMP_destroy(BMP *bmp);
BMP* BMP_clone (BMP *src);
void BMP_copyData(BMP *dest, BMP *src);
void BMP_getPixel(BMP *bmp, int x, int y, unsigned char *r, unsigned char *g, unsigned char *b);
void BMP_setPixel(BMP *bmp, int x, int y, unsigned char r, unsigned char g, unsigned char b);
#endif

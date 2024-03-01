/*
mbmp.h - A small C library for interfacing with BMP files
buzzqrd

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>

*/
#ifndef _MBMP_H
#define _MBMP_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#define BITS_PER_BYTE 8
#define BYTE_ALIGN 4 // align to 4 byte boundaries


typedef struct mbmp_image_t {
	FILE *fp;
	uint32_t size, data_start, compression, data_size, palette;
	uint16_t planes, bits_per_pix;
	int32_t width, height;
	uint8_t cartesian;
	uint64_t print_dimentions;
} mbmp_image_t;

typedef struct mbmp_pixel_t {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha;
} mbmp_pixel_t;


int bmp_open_file(char *filename, mbmp_image_t *file);
int bmp_make_file(char *filename, uint32_t width, uint32_t height, uint32_t color, mbmp_image_t *file);
int bmp_close_file(mbmp_image_t *file);
int bmp_get_pixel(int x, int y, uint32_t *pix, mbmp_image_t *file);
int bmp_set_pixel(int x, int y, uint32_t *pix, mbmp_image_t *file);
void bmp_hex_to_rgb(uint32_t *hex, mbmp_pixel_t *rgb);
void bmp_rgb_to_hex(mbmp_pixel_t *rgb, uint32_t *hex);



#endif

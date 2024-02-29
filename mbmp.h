/*
mbmp.h - A small C library for interfacing with BMP pixels
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



int bmp_open_file(char *filename, mbmp_image_t *file){
	int status = 1, i;
	uint32_t junk;
	file->fp = fopen(filename, "rb+");
	if(file->fp == NULL){
		fprintf(stderr, "mbmp.h: Error: Could not open file \"%s\".\n", filename);
		return(0);
	}
	fseek(file->fp, 0, SEEK_SET);
	/* BMP header */
	/* check for proper magic number in header ("DB") */
	uint16_t magic_num;
	fread(&magic_num, 2, 1, file->fp);
	if(magic_num != 0x4D42){
		fprintf(stderr, "mbmp.h: Error: File \"%s\" was not marked as a BMP file.\n", filename);
		return(0);
	}
	fread(&file->size, 4, 1, file->fp);
	fread(&junk, 2, 2, file->fp);
	fread(&file->data_start, 4, 1, file->fp);
	/* DIB Header */
	uint32_t dib_size;
	fread(&dib_size, 4, 1, file->fp);
	
	/* BITMAPINFOHEADER */
	if(dib_size == 40){
		fread(&file->width, 4, 1, file->fp);
		fread(&file->height, 4, 1, file->fp);
		fread(&file->planes, 2, 1, file->fp);
		fread(&file->bits_per_pix, 2, 1, file->fp);
		fread(&file->compression, 4, 1, file->fp);
		fread(&file->data_size, 4, 1, file->fp);
		fread(&file->print_dimentions, 4, 2, file->fp);
		fread(&file->palette, 4, 1, file->fp);
		/* define vertical direction */
		file->cartesian = 1;
		if(file->height < 0){ file->cartesian = 0; }
		file->height = abs(file->height);
		
	}
	else if(dib_size == 124){
		fread(&file->width, 4, 1, file->fp);
		fread(&file->height, 4, 1, file->fp);
		fread(&file->planes, 2, 1, file->fp);
		fread(&file->bits_per_pix, 2, 1, file->fp);
		fread(&file->compression, 4, 1, file->fp);
		fread(&file->data_size, 4, 1, file->fp);
		fread(&file->print_dimentions, 4, 2, file->fp);
		fread(&file->palette, 4, 1, file->fp);
		/* define vertical direction */
		file->cartesian = 1;
		if(file->height < 0){ file->cartesian = 0; }
		file->height = abs(file->height);
		
		fread(&junk, 4, 1, file->fp); /*Important colors. Not used.*/
		/* check for propper bitmasks */
		for(i=0; i<3; i++){
			fread(&junk, 4, 1, file->fp); /*check for propper masking*/
			if(junk != (0xff0000 >> (i*BITS_PER_BYTE))){
				fprintf(stderr, "Warning: Non-standard RGB layout detected.\n");
				status = 2;
			}
		}

		/* disregard color palettes for now */
	}
	else{
		fprintf(stderr, "mbmp.h: Error: File \"%s\" has a an invalid DIB header size (%d).\n", filename, dib_size);
		return(0);
	}

	return(status);
}

int bmp_make_file(char *filename, uint32_t width, uint32_t height, uint32_t color, mbmp_image_t *file){
	memset(file, 0, sizeof(mbmp_image_t));
	file->fp = fopen(filename, "wb+");
	if(file->fp == NULL){
		fprintf(stderr, "mbmp.h: Error: Could not open a new file.\n");
		return(1);
	}
	
	file->compression = 0;
	file->width = width;
	file->height = height;
	file->palette = 0;
	file->planes = 1;
	file->bits_per_pix = 24;
	file->cartesian = 1;
	file->print_dimentions = 0x130B0000130B0000; /* standard 72 DPI */
	int bytes_per_pix = (int)(file->bits_per_pix / BITS_PER_BYTE);
	int padding_size = (bytes_per_pix * file->width) % BYTE_ALIGN;
	int header_size = 14;
	int dib_size = 40;
	
	file->size = header_size + dib_size + ((width+padding_size) * height);
	file->data_size = ((width+padding_size) * height);	
	file->data_start = header_size + dib_size + 4; /* extra shift */

	uint8_t magic_num[2] = {'B', 'M'};
	uint32_t junk = 0;
	fwrite(magic_num, 1, 2, file->fp);
	fwrite(&file->size, 4, 1, file->fp);	
	fwrite(&junk, 4, 1, file->fp);	
	fwrite(&file->data_start, 4, 1, file->fp);	
	fwrite(&dib_size, 4, 1, file->fp);	
	fwrite(&file->width, 4, 1, file->fp);	
	fwrite(&file->height, 4, 1, file->fp);	
	fwrite(&file->planes, 2, 1, file->fp);	
	fwrite(&file->bits_per_pix, 2, 1, file->fp);	
	fwrite(&file->compression, 4, 1, file->fp);	
	fwrite(&file->data_size, 4, 1, file->fp);	
	fwrite(&file->print_dimentions, 8, 1, file->fp);	
	fwrite(&file->palette, 4, 1, file->fp);	
	fwrite(&file->size, 4, 1, file->fp);	
	fwrite(&junk, 4, 1, file->fp);
	
	int x,y,p,i;
	uint8_t data_byte;
	for(y=0; y<file->height; y++){
		for(x=0; x<file->width; x++){
			for(i=0; i<bytes_per_pix; i++){
				data_byte = (color >> (i*BITS_PER_BYTE)) & 0xff;
				fwrite(&data_byte, 1, 1, file->fp);
			}
		}
		data_byte = 0;
		for(p=0; p<padding_size; p++){
			fwrite(&data_byte, 1, 1, file->fp);
		}
	}

	return(0);
}



int bmp_close_file(mbmp_image_t *file){
	if(file->fp != NULL){
		fclose(file->fp);
	}
	return(0);
}


int bmp_get_pixel(int x, int y, uint32_t *pix, mbmp_image_t *file){
	if(x >= file->width || y >= file->height || x < 0 || y < 0){
		fprintf(stderr, "mbmp.h: Error: bmp_set_pixel x, y is out of bounds. (%d, %d) is not in image of size %dx%d.\n", x, y, file->width, file->height);
		return(0);
	}
	if(file->bits_per_pix > 32){
		fprintf(stderr, "mbmp.h: Error: Too many bits per pixel. Maximum is 32 bits (4 bytes).\n");
		return(0);
	}
	int bytes_per_pix = (int)(file->bits_per_pix / BITS_PER_BYTE);
	int padding_size = (bytes_per_pix * file->width) % BYTE_ALIGN;
	if(file->cartesian){
		y = file->height - y - 1;
	}
	int pixel_index = (y*((bytes_per_pix * file->width)+padding_size)) + (x*bytes_per_pix);
	

	fseek(file->fp, file->data_start+pixel_index, SEEK_SET);
	*pix = 0;
	int i=0;
	for(i=0; i<bytes_per_pix; i++){
		*pix |= ((uint32_t)fgetc(file->fp) << (BITS_PER_BYTE * i));
	}

	return(1);
}


int bmp_set_pixel(int x, int y, uint32_t *pix, mbmp_image_t *file){
	if(x >= file->width || y >= file->height || x < 0 || y < 0){
		fprintf(stderr, "mbmp.h: Error: bmp_get_pixel x, y is out of bounds. (%d, %d) is not in image of size %dx%d.\n", x, y, file->width, file->height);
		return(0);
	}
	if(file->bits_per_pix > 32){
		fprintf(stderr, "mbmp.h: Error: Too many bits per pixel. Maximum is 32 bits (4 bytes).\n");
		return(0);
	}
	int bytes_per_pix = (int)(file->bits_per_pix / BITS_PER_BYTE);
	int padding_size = (bytes_per_pix * file->width) % BYTE_ALIGN;
	if(file->cartesian){
		y = file->height - y - 1;
	}
	int pixel_index = (y*((bytes_per_pix * file->width)+padding_size)) + (x*bytes_per_pix);
	
	fseek(file->fp, file->data_start+pixel_index, SEEK_SET);
	int i;
	uint8_t c;
	for(i=0; i<bytes_per_pix; i++){
		c = (*pix >> (i*BITS_PER_BYTE)) & 0xff;
		fwrite(&c, 1, 1, file->fp);
	}

	return(1);
}

void bmp_hex_to_rgb(uint32_t *hex, mbmp_pixel_t *rgb){
	rgb->alpha = (*hex & 0xFF000000) >> 24;
	rgb->red   = (*hex & 0x00FF0000) >> 16;
	rgb->green = (*hex & 0x0000FF00) >> 8;
	rgb->blue  = (*hex & 0x000000FF);
	return;
}

void bmp_rgb_to_hex(mbmp_pixel_t *rgb, uint32_t *hex){
	*hex = 0;
	*hex |= (uint32_t)rgb->alpha << 24;
	*hex |= (uint32_t)rgb->red << 16;
	*hex |= (uint32_t)rgb->green << 8;
	*hex |= (uint32_t)rgb->blue;
	return;
}


#endif

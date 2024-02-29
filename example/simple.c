#include <stdio.h>
#include <stdlib.h>

#include "../mbmp.h"

int main(void){

	mbmp_image_t img;
	uint32_t pix;

	if(bmp_open_file("snail.bmp", &img) != 1){
		fprintf(stderr, "Error opening BMP file.\n");
	}
	printf("Image size: %dx%d\n", img.width, img.height);

	bmp_get_pixel(50, 50, &pix, &img);
	printf("Pixel color: 0x%08X\n", pix);

	return(0);
}



# mbmp :camera:
A simple drop-in library for reading adn writing to BMP files.
Some formats are not yet supported.

## Installing
To install, just copy ```mbmp.h``` into your project and include it witt ```#include "mbmp.h"```.

## Usage
Example:
```
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

	bmp_close_file(&img); 

	return(0);
}
```



## Contributing
Please submit any pull requests that fix bugs or add any BMP formats/headers that are not currently supported.

## License
The Unlicence.

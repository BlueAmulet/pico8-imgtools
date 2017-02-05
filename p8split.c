#include <stdio.h>
#include <stdlib.h>

#include "lodepng.h"

unsigned char* image;
unsigned char* code_image;
unsigned char* label_image;

void problem(const char* filename, const char* message) {
	free(image);
	free(code_image);
	free(label_image);
	printf("error: %s: %s\n", filename, message);
	exit(1);
}

int main(int argc, char** argv) {
	unsigned error;
	unsigned width, height;

	if (argc != 4) {
		printf("Usage: %s cart.png code.png label.png\n", argv[0]);
		return 1;
	}
	code_image = malloc(160*205*4);
	if (code_image == NULL) {
		printf("error: could not allocate memory\n");
		exit(1);
	}
	label_image = malloc(160*205*4);
	if (label_image == NULL) {
		free(code_image);
		printf("error: could not allocate memory\n");
		exit(1);
	}
	error = lodepng_decode32_file(&image, &width, &height, argv[1]);
	if (error) {
		problem(argv[1], lodepng_error_text(error));
	} else if (width != 160 || height != 205) {
		problem(argv[1], "image has wrong dimensions, expected 160x205");
	}
	int length = 160*205*4;
	for (int i = 0; i < length; i++) {
		unsigned char code = (image[i] & 0x03);
		if (i%4 != 3)
			code_image[i] = code | code << 2 | code << 4 | code << 6;
		else
			code_image[i] = code | 0xFC;
		label_image[i] = (image[i] & 0xFC) | 0x03;
	}
	free(image);
	error = lodepng_encode32_file(argv[2], code_image, 160, 205);
	if (error) {
		problem(argv[2], lodepng_error_text(error));
	}
	free(code_image);
	error = lodepng_encode32_file(argv[3], label_image, 160, 205);
	if (error) {
		problem(argv[3], lodepng_error_text(error));
	}
	free(label_image);
	return 0;
}

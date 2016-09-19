#include <stdio.h>
#include <stdlib.h>

#include "lodepng.h"

unsigned char* code_image;
unsigned char* label_image;

void problem(const char* filename, const char* message) {
	free(code_image);
	free(label_image);
	printf("error: %s: %s\n", filename, message);
	exit(1);
}

int main(int argc, char** argv) {
	unsigned error;
	unsigned width, height;

	if (argc != 4) {
		printf("Usage: %s code.png label.png output.png\n", argv[0]);
		return 1;
	}
	error = lodepng_decode32_file(&code_image, &width, &height, argv[1]);
	if (error) {
		problem(argv[1], lodepng_error_text(error));
	} else if (width != 160 || height != 205) {
		problem(argv[1], "image has wrong dimensions, expected 160x205");
	}
	error = lodepng_decode32_file(&label_image, &width, &height, argv[2]);
	if (error) {
		problem(argv[2], lodepng_error_text(error));
	} else if (width != 160 || height != 205) {
		problem(argv[2], "image has wrong dimensions, expected 160x205");
	}
	int length = 160*205*4;
	for (int i = 0; i < length; i++) {
		code_image[i] = (label_image[i] & 0xFC) | (code_image[i] & 0x03);
	}
	free(label_image);
	error = lodepng_encode32_file(argv[3], code_image, 160, 205);
	if (error) {
		problem(argv[3], lodepng_error_text(error));
	}
	free(code_image);
	return 0;
}

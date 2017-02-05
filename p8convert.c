#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "lodepng.h"

unsigned char* data_image;
unsigned char* data_binary;

void problem(const char* filename, const char* message) {
	free(data_image);
	free(data_binary);
	printf("error: %s: %s\n", filename, message);
	exit(1);
}

unsigned char zukat(unsigned char in) {
	return in | in << 2 | in << 4 | in << 6;
}

int main(int argc, char** argv) {
	unsigned error;
	unsigned width, height;

	if (argc != 3) {
		printf("Usage: %s input.(png|bin) output.(bin|png)\n", argv[0]);
		return 1;
	}
	const char* inext = strrchr(argv[1], '.');
	const char* outext = strrchr(argv[2], '.');
	if (!inext) {
		problem(argv[1], "file has no extension");
	} else if (!outext) {
		problem(argv[2], "file has no extension");
	} else if (!strcmp(inext, ".png")) {
		if (strcmp(outext, ".bin")) {
			problem(argv[2], "output filename must end in '.bin'");
		}
		data_binary = malloc(0x8020);
		if (data_binary == NULL) {
			printf("error: could not allocate memory\n");
			exit(1);
		}
		error = lodepng_decode32_file(&data_image, &width, &height, argv[1]);
		if (error) {
			problem(argv[1], lodepng_error_text(error));
		} else if (width != 160 || height != 205) {
			problem(argv[1], "image has wrong dimensions, expected 160x205");
		}
		int length = 0x8020;
		for (int i = 0; i < length; i++) {
			data_binary[i] =
				(data_image[i*4+2] & 0x03) |
				(data_image[i*4+1] & 0x03) << 2 |
				(data_image[i*4+0] & 0x03) << 4 |
				(data_image[i*4+3] & 0x03) << 6;
		}
		free(data_image);
		FILE* outfile = fopen(argv[2], "wb");
		if (outfile == NULL) {
			free(data_binary);
			printf("error: %s: failed to open file for output: %s\n", argv[2], strerror(errno));
			exit(1);
		}
		fwrite(data_binary, 1, 0x8020, outfile);
    	if (ferror(outfile)) {
			printf("error: %s: problem writing to file\n", argv[2]);
		}
		fclose(outfile);
		free(data_binary);
	} else if (!strcmp(inext, ".bin")) {
		if (strcmp(outext, ".png")) {
			problem(argv[2], "output filename must end in '.png'");
		}
		data_image = malloc(160*205*4);
		if (data_image == NULL) {
			printf("error: could not allocate memory\n");
			exit(1);
		}
		data_binary = malloc(0x8020);
		if (data_binary == NULL) {
			free(data_image);
			printf("error: could not allocate memory\n");
			exit(1);
		}
		FILE* infile = fopen(argv[1], "rb");
		if (infile == NULL) {
			free(data_image);
			free(data_binary);
			printf("error: %s: failed to open file for input: %s\n", argv[1], strerror(errno));
			exit(1);
		}
	    fseek(infile, 0, SEEK_END);
		if (ftell(infile) != 0x8020) {
			fclose(infile);
			problem(argv[2], "input file has wrong size, expected 32800 bytes");
		}
	    fseek(infile, 0, SEEK_SET);
		fread(data_binary, 1, 0x8020, infile);
    	if (ferror(infile)) {
			fclose(infile);
			problem(argv[1], "problem reading from file");
		}
		fclose(infile);
		int length = 0x8020;
		for (int i = 0; i < length; i++) {
			data_image[i*4+2] = zukat(data_binary[i] & 0x03);
			data_image[i*4+1] = zukat((data_binary[i] & 0x0c) >> 2);
			data_image[i*4+0] = zukat((data_binary[i] & 0x30) >> 4);
			data_image[i*4+3] = (data_binary[i] & 0xc0) >> 6 | 0xFC;
		}
		free(data_binary);
		error = lodepng_encode32_file(argv[2], data_image, 160, 205);
		if (error) {
			problem(argv[2], lodepng_error_text(error));
		}
		free(data_image);
	} else {
		problem(argv[1], "unrecognized file extension");
	}
	return 0;
}

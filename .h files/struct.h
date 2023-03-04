//Copyright Ariana-Maria Lazar-Andrei 312CAb 2022-2023
#pragma once
#define PIXELS 256
#define MIN 0

typedef struct {
	int r, g, b;
} color_ascii;

typedef union {
	color_ascii rgb;
	int graysc;
} pixel_type_ascii;

typedef struct {
	unsigned char r, g, b;
} color_bin;

typedef union {
	color_bin rgb;
	unsigned char graysc;
} pixel_type_bin;

typedef struct {
	char magicword[3];
	int width;
	int height;
	int valmax;
	pixel_type_ascii **data;

} img_ascii;

typedef struct {
	char magicword[3];
	int width;
	int height;
	int valmax;
	pixel_type_bin **data;
} img_bin;

typedef union {
	img_bin imgb;
	img_ascii imga;
} img_type;

typedef struct {
	int origin_abs;
	int origin_ord;
	int width;
	int height;
} zone;

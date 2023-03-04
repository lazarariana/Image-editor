//Copyright Ariana-Maria Lazar-Andrei 312CAb 2022-2023
#pragma once
#define PIXELS 256
#define MIN 0

pixel_type_ascii **create_ascii(int width, int height);
pixel_type_bin **create_bin(int width, int height);
void read_matrix_ascii(FILE *f, img_type *img, char filename[]);
void read_matrix_bin(FILE *f, img_type *img, char filename[]);
void free_ascii(pixel_type_ascii **array_ascii, int height);
void free_bin(pixel_type_bin **array_bin, int height);
void skip_comm(FILE *fptr);
void read_ascii(FILE *f, img_type *img);
void read_bin(FILE **f, img_type *img);
bool is_binary(FILE **f, char magic_word[], char filename[], char status[]);
void swap(int *a, int *b);
void build_cr_bin(img_type *img, zone *selected_zone,
				  pixel_type_bin **crop_bin, int width, int height,
				  char status[]);
void build_cr_ascii(img_type *img, zone *selected_zone,
					pixel_type_ascii **crop_bin, int width, int height,
					char status[]);
int clamp(int *a);
void build_freq(img_type *img, char status[], long long freq[], int group,
				long long *max);
bool verif(char first[]);
bool digits(char first[]);
void read_command(char command[], char status[]);
void edge_sharpen_ascii(img_type *img, zone selected_zone,
						pixel_type_ascii **array_ascii, int lee_lin[],
						int lee_col[], double mat_filter[]);
void edge_sharpen_bin(img_type *img, zone selected_zone,
					  pixel_type_ascii **array_ascii, int lee_lin[],
					  int lee_col[], double mat_filter[]);
void blur_ascii(img_type *img, zone selected_zone,
				pixel_type_ascii **array_ascii, int lee_lin[], int lee_col[],
				double mat_filter[], double fr);
void blur_bin(img_type *img, zone selected_zone, pixel_type_ascii **array_ascii,
			  int lee_lin[], int lee_col[], double mat_filter[], double fr);
void bin_to_ascii(img_type *img, char filename[]);
void bin_to_bin(img_type *img, char filename[]);
void ascii_to_ascii(img_type *img, char filename[]);
void ascii_to_bin(img_type *img, char filename[]);
void rotate(img_type *img, char status[], zone *selected_zone);
void cnt_rotate(img_type *img, char status[], zone *selected_zone, int cnt);
void rotate_90(img_type *img, char status[], zone *selected_zone);
void copy_matrix_bin(img_type *img, pixel_type_bin **aux, zone *selected_zone,
					 char status[]);
void copy_matrix_ascii(img_type *img, pixel_type_ascii **aux,
					   zone *selected_zone, char status[]);

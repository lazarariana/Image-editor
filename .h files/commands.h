//Copyright Ariana-Maria Lazar-Andrei 312CAb 2022-2023
#pragma once
#define PIXELS 256
#define MIN 0

void exit_all(char status[], img_type * img, bool print);
void load(FILE *f, img_type *img, char magic_word[], char filename[]);
void select_zone(char status[], img_type *img, zone *selected_zone, int x1,
				 int y1, int x2, int y2, bool print);
void select_all(char status[], img_type *img, zone *selected_zone, bool print);
void crop(char status[], img_type *img, zone *selected_zone);
void apply(char status[], img_type *img, zone selected_zone);
void save(img_type *img, char filename[], char status[], char save_type[]);
void histogram(img_type *img, char status[]);
void equalize_graysc(char status[], img_type *img);

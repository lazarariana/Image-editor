//Copyright Ariana-Maria Lazar-Andrei 312CAb 2022-2023
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "struct.h"
#include "commands.h"
#include "edit_functions.h"

#define PIXELS 256
#define MIN 0
#define MAX_LENGHT 30

/*Dezaloca matricea de pixeli*/
void exit_all(char status[], img_type *img, bool print)
{
	if (strcmp(status, "empty") == 0 && print == 1) {
		printf("No image loaded\n");
		return;
	}
	if (strcmp(status, "binary") == 0) {
		for (int i = 0; i < img->imgb.height; i++)
			free(img->imgb.data[i]);
		free(img->imgb.data);
		img->imgb.data = NULL;
	} else {
		for (int i = 0; i < img->imga.height; i++)
			free(img->imga.data[i]);
		free(img->imga.data);
		img->imga.data = NULL;
	}
}

/*Incarcam imaginea din fisierul ascii/binar in memorie.
Daca exista deja o imagine, aceasta este inlocuita*/
void load(FILE *f, img_type *img, char magic_word[], char filename[])
{
	if (magic_word[1] <= '3')
		strcpy(img->imga.magicword, magic_word);
	else
		strcpy(img->imgb.magicword, magic_word);

	switch (magic_word[1]) {
	case '2':
	case '3':
		read_ascii(f, img);
		break;
	case '5':
	case '6':
		read_bin(&f, img);
		break;
	default:
		printf("Invalid command\n");
	}

	if (magic_word[1] == '2' || magic_word[1] == '3')
		read_matrix_ascii(f, img, filename);
	else
		read_matrix_bin(f, img, filename);
	fclose(f);
}

/*realizeaza o selectie din imagine daca coordonatele sunt valide*/
void select_zone(char status[], img_type *img, zone *selected_zone, int x1,
				 int y1, int x2, int y2, bool print)
{
	if (x1 > x2)
		swap(&x1, &x2);
	if (y1 > y2)
		swap(&y1, &y2);

	if (strcmp(status, "binary") == 0) {
		if (x1 < 0 || y1 < 0 || x2 > img->imgb.width || y2 > img->imgb.height ||
			x1 == x2 || y1 == y2) {
			printf("Invalid set of coordinates\n");
			return;
		}

	} else {
		if (x1 < 0 || y1 < 0 || x2 > img->imga.width || y2 > img->imga.height ||
			x1 == x2 || y1 == y2) {
			printf("Invalid set of coordinates\n");
			return;
		}
	}

	if (strcmp(status, "empty") == 0) {
		printf("No image loaded\n");
		return;
	}

	selected_zone->origin_abs = x1;
	selected_zone->origin_ord = y1;
	selected_zone->height = y2 - y1;
	selected_zone->width = x2 - x1;

	if (print)
		printf("Selected %d %d %d %d\n", x1, y1, x2, y2);
}

/*selecteaza toata zona de memorie alocata imaginii*/
void select_all(char status[], img_type *img, zone *selected_zone, bool print)
{
	if (strcmp(status, "empty") == 0) {
		printf("No image loaded\n");
		return;
	}

	selected_zone->origin_abs = 0;
	selected_zone->origin_ord = 0;
	if (strcmp(status, "binary") == 0) {
		selected_zone->height = img->imgb.height;
		selected_zone->width = img->imgb.width;
	} else {
		selected_zone->height = img->imga.height;
		selected_zone->width = img->imga.width;
	}
	if (print)
		printf("Selected ALL\n");
}

/*inlocuieste in memorie imaginea incarcata cu selectia curenta. Daca selectia
nu este completa, trebuie actualizate campurile din zone si dimensiunile noi
ale imaginii*/
void crop(char status[], img_type *img, zone *selected_zone)
{
	int width, height;

	if (strcmp(status, "empty") == 0) {
		printf("No image loaded\n");
		return;
	}

	width = selected_zone->width;
	height = selected_zone->height;
	if (strcmp("binary", status) == 0) {
		pixel_type_bin **crop_bin = NULL;
		crop_bin = create_bin(width, height);
		build_cr_bin(img, selected_zone, crop_bin, width, height, status);
	} else {
		pixel_type_ascii **crop_ascii = NULL;
		crop_ascii = create_ascii(width, height);
		build_cr_ascii(img, selected_zone, crop_ascii, width,
					   height, status);
	}

	printf("Image cropped\n");
	select_all(status, img, selected_zone, 0);
}

/*aplica un filtru zonei de memorie selectate din imagine. Fiecarui filtru ii
corespunde o matrice stocata intr-un vector de tip double*/
void apply(char status[], img_type *img, zone selected_zone)
{
	int width, height;
	char parameter[MAX_LENGHT], ch;

	scanf("%c", &ch);
	if (ch != ' ') {
		if (strcmp(status, "empty") == 0) {
			printf("No image loaded\n");
			return;
		}
		printf("Invalid command\n");
		return;
	}
	scanf("%s", parameter);
	if (strcmp(status, "empty") == 0) {
		printf("No image loaded\n");
		return;
	}

	if (img->imga.magicword[1] != '3' && img->imgb.magicword[1] != '6') {
		printf("Easy, Charlie Chaplin\n");
		return;
	}
	if (strcmp(parameter, "EDGE") != 0 && strcmp(parameter, "SHARPEN") != 0 &&
		strcmp(parameter, "BLUR") != 0 &&
		strcmp(parameter, "GAUSSIAN_BLUR") != 0) {
		printf("APPLY parameter invalid\n");
		return;
	}

	double edge[] = {-1, -1, -1, -1, 8, -1, -1, -1, -1};
	double sharpen[] = {0, -1, 0, -1, 5, -1, 0, -1, 0};
	double blur[] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
	double gauss_blur[] = {1, 2, 1, 2, 4, 2, 1, 2, 1};
	int lee_lin[] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};
	int lee_col[] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};

	double box_fr = 1.0 / 9.0;
	double gauss_fr = 1.0 / 16.0;
	width = selected_zone.width;
	height = selected_zone.height;
	pixel_type_ascii **array_ascii = NULL;
	array_ascii = create_ascii(width, height);

	if (strcmp(status, "ascii") == 0) {
		if (strcmp(parameter, "EDGE") == 0)
			edge_sharpen_ascii(img, selected_zone, array_ascii, lee_lin,
							   lee_col, edge);
		if (strcmp(parameter, "SHARPEN") == 0)
			edge_sharpen_ascii(img, selected_zone, array_ascii, lee_lin,
							   lee_col, sharpen);
		if (strcmp(parameter, "BLUR") == 0)
			blur_ascii(img, selected_zone, array_ascii, lee_lin, lee_col, blur,
					   box_fr);
		if (strcmp(parameter, "GAUSSIAN_BLUR") == 0)
			blur_ascii(img, selected_zone, array_ascii, lee_lin, lee_col,
					   gauss_blur, gauss_fr);
	} else {
		if (strcmp(parameter, "EDGE") == 0)
			edge_sharpen_bin(img, selected_zone, array_ascii, lee_lin, lee_col,
							 edge);
		if (strcmp(parameter, "SHARPEN") == 0)
			edge_sharpen_bin(img, selected_zone, array_ascii, lee_lin, lee_col,
							 sharpen);
		if (strcmp(parameter, "BLUR") == 0)
			blur_bin(img, selected_zone, array_ascii, lee_lin, lee_col, blur,
					 box_fr);
		if (strcmp(parameter, "GAUSSIAN_BLUR") == 0)
			blur_bin(img, selected_zone, array_ascii, lee_lin, lee_col,
					 gauss_blur, gauss_fr);
	}
	printf("APPLY %s done\n", parameter);
	free_ascii(array_ascii, height);
}

/*salveaza imaginea curenta intr-un fisier*/
void save(img_type *img, char filename[], char status[], char save_type[])
{
	if (strcmp(status, "empty") == 0) {
		printf("No image loaded\n");
		return;
	}

	if (strcmp(status, "binary") == 0 && strcmp(save_type, "binary") == 0)
		bin_to_bin(img, filename);
	if (strcmp(status, "ascii") == 0 && strcmp(save_type, "ascii") == 0)
		ascii_to_ascii(img, filename);
	if (strcmp(status, "ascii") == 0 && strcmp(save_type, "binary") == 0)
		ascii_to_bin(img, filename);
	if (strcmp(status, "binary") == 0 && strcmp(save_type, "ascii") == 0)
		bin_to_ascii(img, filename);

	printf("Saved %s\n", filename);
}

/*realizeaza histograma imaginii cu y bin-uri si maxim x stelute*/
void histogram(img_type *img, char status[])
{
	long long freq[PIXELS] = {0}, max = -1;
	int group, nr_stars, k = 0, x, y;
	char ch, first[MAX_LENGHT];
	bool all_valid = true;

	scanf("%c", &ch);
	if (ch == '\n') {
		if (strcmp(status, "empty") == 0) {
			printf("No image loaded\n");
			return;
		}
		printf("Invalid command\n");
		return;
	}
	while (ch == ' ') {
		scanf("%s", first);
		k++;
		switch (k) {
		case 1:
			if (digits(first))
				x = atoi(first);
			else
				all_valid = false;
			break;
		case 2:
			if (digits(first))
				y = atoi(first);
			else
				all_valid = false;
			break;
		}
		scanf("%c", &ch);
	}
	if (all_valid && k == 2) {
		if (strcmp(status, "empty") == 0) {
			printf("No image loaded\n");
		} else {
			if (strcmp(status, "binary") == 0 &&
				img->imgb.magicword[1] != '5') {
				printf("Black and white image needed\n");
				return;
			}
			if (strcmp(status, "ascii") == 0 && img->imga.magicword[1] != '2') {
				printf("Black and white image needed\n");
				return;
			}
			if (!(y >= 2 && y <= 256) || (y & (y - 1)) != 0) {
				printf("Invalid parameters\n");
				return;
			}

			group = PIXELS / y;
			build_freq(img, status, freq, group, &max);

			for (int i = 0; i < y; i++) {
				nr_stars = floor((double)freq[i] / max * x);
				printf("%d\t|\t", nr_stars);
				for (int j = 0; j < nr_stars; j++)
					printf("*");
				printf("\n");
			}
		}
	} else {
		if (strcmp(status, "empty") == 0)
			printf("No image loaded\n");
		printf("Invalid command\n");
	}
}

/*egalizeaza imaginea conform formulei din enunt dupa formarea vectorului de
frecvente*/
void equalize_graysc(char status[], img_type *img)
{
	if (strcmp(status, "binary") == 0 && img->imgb.magicword[1] != '5') {
		printf("Black and white image needed\n");
		return;
	}

	if (strcmp(status, "ascii") == 0 && img->imga.magicword[1] != '2') {
		printf("Black and white image needed\n");
		return;
	}

	if (strcmp(status, "empty") == 0) {
		printf("No image loaded\n");
		return;
	}

	long long freq[PIXELS] = {0};
	double area, sum_h = 0;

	if (strcmp(status, "binary") == 0) {
		for (int i = 0; i < img->imgb.height; i++)
			for (int j = 0; j < img->imgb.width; j++)
				freq[img->imgb.data[i][j].graysc]++;
		area = img->imgb.height * img->imgb.width;
		for (int i = 0; i < img->imgb.height; i++)
			for (int j = 0; j < img->imgb.width; j++) {
				sum_h = 0;
				for (int k = 0; k <= img->imgb.data[i][j].graysc; k++)
					sum_h += freq[k];
				img->imgb.data[i][j].graysc =
					round((double)sum_h / area * (PIXELS - 1));
			}
	} else {
		for (int i = 0; i < img->imga.height; i++)
			for (int j = 0; j < img->imga.width; j++)
				freq[(long long)img->imga.data[i][j].graysc]++;
		area = img->imga.height * img->imga.width;
		for (int i = 0; i < img->imga.height; i++)
			for (int j = 0; j < img->imga.width; j++) {
				sum_h = 0;
				for (int k = 0; k <= img->imga.data[i][j].graysc; k++)
					sum_h += freq[k];
				img->imga.data[i][j].graysc =
					round((double)sum_h / area * (PIXELS - 1));
			}
	}
	printf("Equalize done\n");
}

/*roteste imaginea la unghiuri cu masura unui multiplu intreg de 90 cuprins in
intervalul [-360, 360]*/
void rotate(img_type *img, char status[], zone *selected_zone)
{
	int angle, height, width, imgh, imgw;
	int cnt;

	width = selected_zone->width;
	height = selected_zone->height;
	imgh = img->imgb.height;
	imgw = img->imga.width;
	if (strcmp(status, "binary") == 0)
		if (height != imgh && width != imgw && height != width) {
			printf("The selection must be square\n");
			return;
		}
	if (strcmp(status, "ascii") == 0)
		if (height != imgh && width != imgw && height != width) {
			printf("The selection must be square\n");
			return;
		}

	scanf("%d", &angle);
	if (angle > 0)
		cnt = angle / 90;
	else
		cnt = (360 + angle) / 90;

	switch (angle) {
	case 90:
	case -270:
	case -90:
	case 270:
	case 180:
	case -180:
		if (strcmp(status, "empty") == 0) {
			printf("No image loaded\n");
			return;
		}
		cnt_rotate(img, status, selected_zone, cnt);
		printf("Rotated %d\n", angle);
		break;
	case 360:
	case -360:
	case 0:
		if (strcmp(status, "empty") == 0) {
			printf("No image loaded\n");
			return;
		}
		printf("Rotated %d\n", angle);
		break;
	default:
		if (strcmp(status, "empty") == 0) {
			printf("No image loaded\n");
			return;
		}
		printf("Unsupported rotation angle\n");
		return;
	}
	if (strcmp(status, "empty") == 0) {
		printf("No image loaded\n");
		return;
	}
}

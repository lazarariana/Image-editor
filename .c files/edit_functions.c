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
#define LINE 100
#define MAX_LENGHT 30

/*Alocam dinamic memorie pentru o matrice de lungime width si inaltime height
in functie de tipul fisierului*/
pixel_type_ascii **create_ascii(int width, int height)
{
	pixel_type_ascii **array = NULL;
	array = (pixel_type_ascii **)malloc(height * sizeof(pixel_type_ascii *));
	if (!array)
		return NULL;

	for (int i = 0; i < height; i++) {
		array[i] = (pixel_type_ascii *)calloc(width, sizeof(pixel_type_ascii));
		if (!array[i]) {
			for (int j = i - 1; j >= 0; j--)
				free(array[j]);
			free(array);
			return NULL;
		}
	}
	return array;
}

pixel_type_bin **create_bin(int width, int height)
{
	pixel_type_bin **array = NULL;
	array = (pixel_type_bin **)malloc(height * sizeof(pixel_type_bin *));
	if (!array)
		return NULL;

	for (int i = 0; i < height; i++) {
		array[i] = (pixel_type_bin *)calloc(width, sizeof(pixel_type_bin));
		if (!array[i]) {
			for (int j = i - 1; j >= 0; j--)
				free(array[j]);
			free(array);
			return NULL;
		}
	}
	return array;
}

/*Sunt necesare pentru comanda LOAD, citind matricea de pixeli ce trebuie sa
corespunda campului data.*/
void read_matrix_ascii(FILE *f, img_type *img, char filename[])
{
	int width, height;
	width = img->imga.width;
	height = img->imga.height;
	pixel_type_ascii **array = NULL;
	array = create_ascii(width, height);
	if (!array)
		printf("Failed to load %s\n", filename);
	else
		for (int i = 0; i < img->imga.height; i++)
			for (int j = 0; j < img->imga.width; j++)
				if (img->imga.magicword[1] == '3')
					fscanf(f, "%d%d%d", &array[i][j].rgb.r,
						   &array[i][j].rgb.g, &array[i][j].rgb.b);
				else
					fscanf(f, "%d", &array[i][j].graysc);
	img->imga.data = array;
	printf("Loaded %s\n", filename);
}

void read_matrix_bin(FILE *f, img_type *img, char filename[])
{
	int width, height;
	width = img->imgb.width;
	height = img->imgb.height;
	pixel_type_bin **array = NULL;
	array = create_bin(width, height);
	if (!array)
		printf("Failed to load %s\n", filename);
	else
		for (int i = 0; i < img->imgb.height; i++)
			for (int j = 0; j < img->imgb.width; j++)
				if (img->imgb.magicword[1] == '6')
					fscanf(f, "%c%c%c", &array[i][j].rgb.r,
						   &array[i][j].rgb.g, &array[i][j].rgb.b);
				else
					fscanf(f, "%c", &array[i][j].graysc);
	img->imgb.data = array;
	printf("Loaded %s\n", filename);
}

/*Dezalocam memoria alocata pentru o matrice in functie de tipul fisierului.*/
void free_ascii(pixel_type_ascii **array_ascii, int height)
{
	for (int i = 0; i < height; i++) {
		free(array_ascii[i]);
		array_ascii[i] = NULL;
	}
	free(array_ascii);
	array_ascii = NULL;
}

void free_bin(pixel_type_bin **array_bin, int height)
{
	for (int i = 0; i < height; i++) {
		free(array_bin[i]);
		array_bin[i] = NULL;
	}
	free(array_bin);
	array_bin = NULL;
}

/*Ignoram comentariile marcate cu '#' la inceput de linie, cat si
blankline-urile, efectuand citirea din fisier caracter cu caracter.*/
void skip_comm(FILE *fptr)
{
	int ch;
	char line[LINE];

	while ((ch = fgetc(fptr)) != EOF && isspace(ch))
		;

	if (ch == '#') {
		fgets(line, sizeof(line), fptr);
		skip_comm(fptr);
	} else {
		fseek(fptr, -1, SEEK_CUR);
	}
}

/* Citim metadatele fisierului.*/
void read_ascii(FILE *f, img_type *img)
{
	skip_comm(f);
	fscanf(f, "%d", &img->imga.width);
	skip_comm(f);
	fscanf(f, "%d", &img->imga.height);
	skip_comm(f);
	fscanf(f, "%d", &img->imga.valmax);
	skip_comm(f);
}

void read_bin(FILE **f, img_type *img)
{
	skip_comm((*f));
	fscanf((*f), "%d", &img->imgb.width);
	skip_comm((*f));
	fscanf((*f), "%d", &img->imgb.height);
	skip_comm((*f));
	fscanf((*f), "%d", &img->imgb.valmax);
	skip_comm((*f));
}

/*Stringul status stocheaza in memorie in ce stadiu se afla fisierul: nu
contine nimic, respectiv un fisier ascii sau binar. Initial deschidem fisierul
presupunand ca este de tip text/ascii. Daca magic wordul contine cifra 4,
respectiv 5 sau 6, atunci fisierul trebuie inchis si redeschis binar.*/
bool is_binary(FILE **f, char magic_word[], char filename[], char status[])
{
	if (strchr("56", magic_word[1])) {
		fclose((*f));
		(*f) = fopen(filename, "rb");
		strcpy(status, "binary");
		fscanf((*f), "%s", magic_word);
		return true;
	}
	strcpy(status, "ascii");
	return false;
}

void swap(int *a, int *b)
{
	int aux = (*a);
	(*a) = (*b);
	(*b) = aux;
}

/*Atribuie pe pozitiile corespunzatoare din matricea auxiliara
crop_bin/crop_ascii valorile din selectia curenta pe care dorim sa o cropam.
Apoi, dezaloca memoria alocata matricei initiale din campul data si o
actualizeaza cu selectia cropata. De asemenea, originea selectiei devine
punctul (0, 0), deoarece selectia este chiar intregul fisier. */
void build_cr_bin(img_type *img, zone *selected_zone,
				  pixel_type_bin **crop_bin, int width, int height,
				  char status[])
{
	int ord, abs;
	ord = selected_zone->origin_ord;
	abs = selected_zone->origin_abs;
	for (int i = ord; i < ord + height; i++)
		for (int j = abs; j < abs + width; j++)
			if (img->imgb.magicword[1] != '6') {
				crop_bin[i - ord][j - abs].graysc =
				img->imgb.data[i][j].graysc;
			} else {
				crop_bin[i - ord][j - abs].rgb.r += img->imgb.data[i][j].rgb.r;
				crop_bin[i - ord][j - abs].rgb.g += img->imgb.data[i][j].rgb.g;
				crop_bin[i - ord][j - abs].rgb.b += img->imgb.data[i][j].rgb.b;
			}
	exit_all(status, img, 1);
	selected_zone->origin_abs = 0;
	selected_zone->origin_ord = 0;
	img->imgb.width = selected_zone->width;
	img->imgb.height = selected_zone->height;
	img->imgb.data = crop_bin;
}

void build_cr_ascii(img_type *img, zone *selected_zone,
					pixel_type_ascii **crop_bin, int width, int height,
					char status[])
{
	int ord, abs;
	ord = selected_zone->origin_ord;
	abs = selected_zone->origin_abs;
	for (int i = ord; i < ord + height; i++)
		for (int j = abs; j < abs + width; j++)
			if (img->imga.magicword[1] != '3') {
				crop_bin[i - ord][j - abs].graysc =
				img->imga.data[i][j].graysc;
			} else {
				crop_bin[i - ord][j - abs].rgb.r += img->imga.data[i][j].rgb.r;
				crop_bin[i - ord][j - abs].rgb.g += img->imga.data[i][j].rgb.g;
				crop_bin[i - ord][j - abs].rgb.b += img->imga.data[i][j].rgb.b;
			}
	exit_all(status, img, 1);
	selected_zone->origin_abs = 0;
	selected_zone->origin_ord = 0;
	img->imga.width = selected_zone->width;
	img->imga.height = selected_zone->height;
	img->imga.data = crop_bin;
}

/*respecta intervalul corespunzator de valori ale pixelilor*/
int clamp(int *a)
{
	if ((*a) < MIN)
		return MIN;
	if ((*a) > PIXELS - 1)
		return (PIXELS - 1);
	return *a;
}

/*am utilizat metoda de parcurgere conform algoritmului lui Lee pentru a accesa
rapid vecinii fiecarui element necesari construirii matricei fiecarui filtru*/
void edge_sharpen_ascii(img_type *img, zone selected_zone,
						pixel_type_ascii **array_ascii, int lee_lin[],
						int lee_col[], double mat_filter[])
{
	int i, j, k;

	for (i = selected_zone.origin_ord;
		 i < selected_zone.origin_ord + selected_zone.height; i++)
		for (j = selected_zone.origin_abs;
			 j < selected_zone.origin_abs + selected_zone.width; j++)
			if (i > 0 && i < img->imgb.height - 1 && j > 0 &&
				j < img->imgb.width - 1) {
				for (k = 0; k < 9; k++) {
					array_ascii[i - selected_zone.origin_ord]
							   [j - selected_zone.origin_abs]
								   .rgb.r +=
						img->imga.data[i + lee_lin[k]][j + lee_col[k]].rgb.r *
						mat_filter[k];
					array_ascii[i - selected_zone.origin_ord]
							   [j - selected_zone.origin_abs]
								   .rgb.g +=
						img->imga.data[i + lee_lin[k]][j + lee_col[k]].rgb.g *
						mat_filter[k];
					array_ascii[i - selected_zone.origin_ord]
							   [j - selected_zone.origin_abs]
								   .rgb.b +=
						img->imga.data[i + lee_lin[k]][j + lee_col[k]].rgb.b *
						mat_filter[k];
				}
			}

	for (i = selected_zone.origin_ord;
		 i < selected_zone.origin_ord + selected_zone.height; i++)
		for (j = selected_zone.origin_abs;
			 j < selected_zone.origin_abs + selected_zone.width; j++)
			if (i > 0 && i < img->imgb.height - 1 && j > 0 &&
				j < img->imgb.width - 1) {
				img->imga.data[i][j].rgb.r =
					clamp(&array_ascii[i - selected_zone.origin_ord]
									  [j - selected_zone.origin_abs]
										  .rgb.r);
				img->imga.data[i][j].rgb.g =
					clamp(&array_ascii[i - selected_zone.origin_ord]
									  [j - selected_zone.origin_abs]
										  .rgb.g);
				img->imga.data[i][j].rgb.b =
					clamp(&array_ascii[i - selected_zone.origin_ord]
									  [j - selected_zone.origin_abs]
										  .rgb.b);
			}
}

void edge_sharpen_bin(img_type *img, zone selected_zone,
					  pixel_type_ascii **array_ascii, int lee_lin[],
					  int lee_col[], double mat_filter[])
{
	int i, j, k;

	for (i = selected_zone.origin_ord;
		 i < selected_zone.origin_ord + selected_zone.height; i++)
		for (j = selected_zone.origin_abs;
			 j < selected_zone.origin_abs + selected_zone.width; j++)
			for (k = 0; k < 9; k++)
				if (i > 0 && i < img->imgb.height - 1 && j > 0 &&
					j < img->imgb.width - 1) {
					array_ascii[i - selected_zone.origin_ord]
							   [j - selected_zone.origin_abs]
								   .rgb.r +=
						img->imgb.data[i + lee_lin[k]][j + lee_col[k]].rgb.r *
						mat_filter[k];
					array_ascii[i - selected_zone.origin_ord]
							   [j - selected_zone.origin_abs]
								   .rgb.g +=
						img->imgb.data[i + lee_lin[k]][j + lee_col[k]].rgb.g *
						mat_filter[k];
					array_ascii[i - selected_zone.origin_ord]
							   [j - selected_zone.origin_abs]
								   .rgb.b +=
						img->imgb.data[i + lee_lin[k]][j + lee_col[k]].rgb.b *
						mat_filter[k];
				}

	for (i = selected_zone.origin_ord;
		 i < selected_zone.origin_ord + selected_zone.height; i++)
		for (j = selected_zone.origin_abs;
			 j < selected_zone.origin_abs + selected_zone.width; j++)
			if (i > 0 && i < img->imgb.height - 1 && j > 0 &&
				j < img->imgb.width - 1) {
				img->imgb.data[i][j].rgb.r =
					clamp(&array_ascii[i - selected_zone.origin_ord]
									  [j - selected_zone.origin_abs]
										  .rgb.r);
				img->imgb.data[i][j].rgb.g =
					clamp(&array_ascii[i - selected_zone.origin_ord]
									  [j - selected_zone.origin_abs]
										  .rgb.g);
				img->imgb.data[i][j].rgb.b =
					clamp(&array_ascii[i - selected_zone.origin_ord]
									  [j - selected_zone.origin_abs]
										  .rgb.b);
			}
}

void blur_ascii(img_type *img, zone selected_zone,
				pixel_type_ascii **array_ascii, int lee_lin[], int lee_col[],
				double mat_filter[], double fr)
{
	int i, j, k;
	for (i = selected_zone.origin_ord;
		 i < selected_zone.origin_ord + selected_zone.height; i++)
		for (j = selected_zone.origin_abs;
			 j < selected_zone.origin_abs + selected_zone.width; j++)
			for (k = 0; k < 9; k++) {
				if (i > 0 && i < img->imga.height - 1 && j > 0 &&
					j < img->imga.width - 1) {
					array_ascii[i - selected_zone.origin_ord]
							   [j - selected_zone.origin_abs]
								   .rgb.r +=
						img->imga.data[i + lee_lin[k]][j + lee_col[k]].rgb.r *
						mat_filter[k];
					array_ascii[i - selected_zone.origin_ord]
							   [j - selected_zone.origin_abs]
								   .rgb.g +=
						img->imga.data[i + lee_lin[k]][j + lee_col[k]].rgb.g *
						mat_filter[k];
					array_ascii[i - selected_zone.origin_ord]
							   [j - selected_zone.origin_abs]
								   .rgb.b +=
						img->imga.data[i + lee_lin[k]][j + lee_col[k]].rgb.b *
						mat_filter[k];
				}
			}

	for (i = selected_zone.origin_ord;
		 i < selected_zone.origin_ord + selected_zone.height; i++)
		for (j = selected_zone.origin_abs;
			 j < selected_zone.origin_abs + selected_zone.width; j++)
			if (i > 0 && i < img->imgb.height - 1 && j > 0 &&
				j < img->imgb.width - 1) {
				img->imga.data[i][j].rgb.r =
					fr * array_ascii[i - selected_zone.origin_ord]
									[j - selected_zone.origin_abs]
										.rgb.r;
				img->imga.data[i][j].rgb.g =
					fr * array_ascii[i - selected_zone.origin_ord]
									[j - selected_zone.origin_abs]
										.rgb.g;
				img->imga.data[i][j].rgb.b =
					fr * array_ascii[i - selected_zone.origin_ord]
									[j - selected_zone.origin_abs]
										.rgb.b;
			}
}

void blur_bin(img_type *img, zone selected_zone, pixel_type_ascii **array_ascii,
			  int lee_lin[], int lee_col[], double mat_filter[], double fr)
{
	int i, j, k;

	for (i = selected_zone.origin_ord;
		 i < selected_zone.origin_ord + selected_zone.height; i++)
		for (j = selected_zone.origin_abs;
			 j < selected_zone.origin_abs + selected_zone.width; j++)
			for (k = 0; k < 9; k++) {
				if (i > 0 && i < img->imgb.height - 1 && j > 0 &&
					j < img->imgb.width - 1) {
					array_ascii[i - selected_zone.origin_ord]
							   [j - selected_zone.origin_abs]
								   .rgb.r +=
						img->imgb.data[i + lee_lin[k]][j + lee_col[k]].rgb.r *
						mat_filter[k];
					array_ascii[i - selected_zone.origin_ord]
							   [j - selected_zone.origin_abs]
								   .rgb.g +=
						img->imgb.data[i + lee_lin[k]][j + lee_col[k]].rgb.g *
						mat_filter[k];
					array_ascii[i - selected_zone.origin_ord]
							   [j - selected_zone.origin_abs]
								   .rgb.b +=
						img->imgb.data[i + lee_lin[k]][j + lee_col[k]].rgb.b *
						mat_filter[k];
				}
			}

	for (i = selected_zone.origin_ord;
		 i < selected_zone.origin_ord + selected_zone.height; i++)
		for (j = selected_zone.origin_abs;
			 j < selected_zone.origin_abs + selected_zone.width; j++)
			if (i > 0 && i < img->imgb.height - 1 && j > 0 &&
				j < img->imgb.width - 1) {
				img->imgb.data[i][j].rgb.r =
					fr * array_ascii[i - selected_zone.origin_ord]
									[j - selected_zone.origin_abs]
										.rgb.r;
				img->imgb.data[i][j].rgb.g =
					fr * array_ascii[i - selected_zone.origin_ord]
									[j - selected_zone.origin_abs]
										.rgb.g;
				img->imgb.data[i][j].rgb.b =
					fr * array_ascii[i - selected_zone.origin_ord]
									[j - selected_zone.origin_abs]
										.rgb.b;
			}
}

void build_freq(img_type *img, char status[], long long freq[], int group,
				long long *max)
{
	if (strcmp(status, "binary") == 0) {
		for (int i = 0; i < img->imgb.height; i++)
			for (int j = 0; j < img->imgb.width; j++) {
				freq[img->imgb.data[i][j].graysc / group]++;
				if (freq[(int)img->imgb.data[i][j].graysc / group] > (*max))
					(*max) = freq[img->imgb.data[i][j].graysc / group];
			}
	} else {
		for (int i = 0; i < img->imga.height; i++)
			for (int j = 0; j < img->imga.width; j++) {
				freq[img->imga.data[i][j].graysc / group]++;
				if (freq[img->imga.data[i][j].graysc / group] > (*max))
					(*max) = freq[img->imga.data[i][j].graysc / group];
			}
	}
}

/*Daca stringul SELECT este urmat de stringul ALL sau 4 valori int pozitive,
atunci comanda este valida. Daca toate caracterele stringului citit sunt
cifre, atunci el este o coordonata si trebuie convertit la int. Verificam acest
lucru cu functia digits. In caz contrar, daca nu este egal lexicografic cu
stringul ALL, functia verif returneaza false si comanda e invalida.*/
bool verif(char first[])
{
	if (strcmp(first, "ALL") == 0)
		return true;
	return false;
}

bool digits(char first[])
{
	size_t i = 0;
	if (first[0] == '-')
		i = 1;
	for (; i < strlen(first); i++)
		if (!isdigit(first[i]))
			return false;
	return true;
}

void coordinates(img_type *img, zone *selected_zone, char status[])
{
	int x1, y1, x2, y2, k = 0;
	char first[10], ch;
	bool all_valid = true;
	scanf("%c", &ch);
	while (ch == ' ') {
		scanf("%s", first);
		k++;
		bool rez = verif(first);
		if (rez && k == 1) {
			if (strcmp(status, "empty") == 0) {
				printf("No image loaded\n");
				return;
			}
			select_all(status, img, selected_zone, 1);
			return;
		}
		switch (k) {
		case 1:
			if (digits(first))
				x1 = atoi(first);
			else
				all_valid = false;
			break;
		case 2:
			if (digits(first))
				y1 = atoi(first);
			else
				all_valid = false;
			break;
		case 3:
			if (digits(first))
				x2 = atoi(first);
			else
				all_valid = false;
			break;
		case 4:
			if (digits(first))
				y2 = atoi(first);
			else
				all_valid = false;
			break;
		}
		scanf("%c", &ch);
	}
	if (all_valid && k == 4)
		if (strcmp(status, "empty") == 0)
			printf("No image loaded\n");
		else
			select_zone(status, img, selected_zone, x1, y1, x2, y2, 1);
	else
		printf("Invalid command\n");
}

/*salveaza un fisier ascii in format ascii*/
void ascii_to_ascii(img_type *img, char filename[])
{
	FILE *output;

	output = fopen(filename, "wt");

	fprintf(output, "%s\n", img->imga.magicword);
	fprintf(output, "%d %d\n", img->imga.width, img->imga.height);
	fprintf(output, "%d\n", img->imga.valmax);

	for (int i = 0; i < img->imga.height; i++)
		for (int j = 0; j < img->imga.width; j++)
			if (img->imga.magicword[1] == '3')
				fprintf(output, "%d %d %d ", img->imga.data[i][j].rgb.r,
						img->imga.data[i][j].rgb.g, img->imga.data[i][j].rgb.b);
			else
				fprintf(output, "%d ", img->imga.data[i][j].graysc);

	fclose(output);
}

/* salveaza un fisier ascii in format bin*/
void ascii_to_bin(img_type *img, char filename[])
{
	FILE *output;
	output = fopen(filename, "wb");

	if (img->imga.magicword[1] == '2')
		fprintf(output, "%s\n ", "P5");
	else if (img->imga.magicword[1] == '3')
		fprintf(output, "%s\n", "P6");
	fprintf(output, "%d %d\n", img->imga.width, img->imga.height);
	fprintf(output, "%d\n", img->imga.valmax);

	for (int i = 0; i < img->imga.height; i++)
		for (int j = 0; j < img->imga.width; j++) {
			if (img->imga.magicword[1] == '3')
				fprintf(output, "%c%c%c",
						(unsigned char)img->imga.data[i][j].rgb.r,
						(unsigned char)img->imga.data[i][j].rgb.g,
						(unsigned char)img->imga.data[i][j].rgb.b);
			else
				fprintf(output, "%c",
						(unsigned char)img->imga.data[i][j].graysc);
		}
	fclose(output);
}

/*salveaza un fisier binar in format ascii*/
void bin_to_ascii(img_type *img, char filename[])
{
	FILE *output;
	output = fopen(filename, "wt");

	if (img->imgb.magicword[1] == '5')
		fprintf(output, "%s\n", "P2");
	else if (img->imgb.magicword[1] == '6')
		fprintf(output, "%s\n", "P3");
	fprintf(output, "%d %d\n", img->imgb.width, img->imgb.height);
	fprintf(output, "%d\n", img->imgb.valmax);

	for (int i = 0; i < img->imgb.height; i++)
		for (int j = 0; j < img->imgb.width; j++) {
			if (img->imga.magicword[1] == '6')
				fprintf(output, "%d %d %d ", (int)img->imgb.data[i][j].rgb.r,
						(int)img->imgb.data[i][j].rgb.g,
						(int)img->imgb.data[i][j].rgb.b);
			else
				fprintf(output, "%d ", (int)img->imgb.data[i][j].graysc);
		}
	fclose(output);
}

/*salveaza un fisier binar in format binar*/
void bin_to_bin(img_type *img, char filename[])
{
	FILE *output;
	output = fopen(filename, "wb");

	fprintf(output, "%s\n", img->imgb.magicword);
	fprintf(output, "%d %d\n", img->imgb.width, img->imgb.height);
	fprintf(output, "%d\n", img->imgb.valmax);

	for (int i = 0; i < img->imgb.height; i++)
		for (int j = 0; j < img->imgb.width; j++)
			if (img->imgb.magicword[1] == '6') {
				fwrite(&img->imgb.data[i][j].rgb,
					   sizeof(img->imgb.data[i][j].rgb), 1, output);

			} else {
				fwrite(&img->imgb.data[i][j].graysc,
					   sizeof(img->imgb.data[i][j].graysc), 1, output);
			}

	fclose(output);
}

/*copiem intr-un auxilar selectia pozei, iar apoi inlocuim pe pozitiile
corespunzatoare din data valorile din aux. In cazul in care selectia este
completa, data trebuie dezalocata*/
void copy_matrix_ascii(img_type *img, pixel_type_ascii **aux,
					   zone *selected_zone, char status[])
{
	int width, height, ord, abs;
	width = selected_zone->width;
	height = selected_zone->height;
	ord = selected_zone->origin_ord;
	abs = selected_zone->origin_abs;
	if (height == width) {
		for (int i = ord; i < ord + height; i++)
			for (int j = abs; j < abs + width; j++) {
				if (img->imga.magicword[1] != '3')
					memmove(&img->imga.data[i][j].graysc,
							&aux[i - ord][j - abs].graysc,
							sizeof(img->imga.data[i][j].graysc));
				else
					memmove(&img->imga.data[i][j].rgb,
							&aux[i - ord][j - abs].rgb,
							sizeof(img->imga.data[i][j].rgb));
			}
		free_ascii(aux, height);
	} else if (height == img->imga.height && width == img->imga.width) {
		exit_all(status, img, 0);
		img->imga.width = selected_zone->height;
		img->imga.height = selected_zone->width;
		int aux_sel;
		aux_sel = selected_zone->height;
		selected_zone->height = selected_zone->width;
		selected_zone->width = aux_sel;
		img->imga.data = aux;
		aux = NULL;
	}
}

void copy_matrix_bin(img_type *img, pixel_type_bin **aux, zone *selected_zone,
					 char status[])
{
	int width, height, ord, abs;
	width = selected_zone->width;
	height = selected_zone->height;
	ord = selected_zone->origin_ord;
	abs = selected_zone->origin_abs;
	if (height == width) {
		for (int i = ord; i < ord + height; i++)
			for (int j = abs; j < abs + width; j++) {
				if (img->imgb.magicword[1] != '6')
					memmove(&img->imgb.data[i][j].graysc,
							&aux[i - ord][j - abs].graysc,
							sizeof(img->imgb.data[i][j].graysc));
				else
					memmove(&img->imgb.data[i][j].rgb,
							&aux[i - ord][j - abs].rgb,
							sizeof(img->imgb.data[i][j].rgb));
			}
		free_bin(aux, height);
	} else if (height == img->imgb.height && width == img->imgb.width) {
		exit_all(status, img, 0);
		img->imgb.width = selected_zone->height;
		img->imgb.height = selected_zone->width;
		int aux_sel;
		aux_sel = selected_zone->height;
		selected_zone->height = selected_zone->width;
		selected_zone->width = aux_sel;
		img->imgb.data = aux;
		aux = NULL;
	}
}

/*Utilizam faptul ca pentru a obtine rotirea unei matrice la 90 de grade
inversam coloanele transpusei sale. Comparand indicii elementelor din matricea
initiala cu indicii noilor pozitii ale lor din transpusa prelucrata, am obtinut
formula pe care am utilizat-o in alcatuirea matricei rotite in matricea
auxiliara aux. Dupa ce am pozitionat toate elementele in aux, copiem rezultatul
in matricea data. Tinem cont de faptul dimensiunile width si height isi
interschimba valorile cand alocam memorie pentru aux, iar atunci cand doar
secventa patratica trebuie rotita, restul pixelilor din  matrice raman
neschimbati.*/
void rotate_90(img_type *img, char status[], zone *selected_zone)
{
	int w, h, ord, abs;
	w = selected_zone->width;
	h = selected_zone->height;
	ord = selected_zone->origin_ord;
	abs = selected_zone->origin_abs;
	if (strcmp(status, "ascii") == 0) {
		pixel_type_ascii **aux = NULL;
		aux = create_ascii(h, w);
		for (int i = 0; i < w; i++) {
			for (int j = 0; j < h; j++) {
				if (img->imga.magicword[1] != '3') {
					memmove(&aux[i][j].graysc,
							&img->imga.data[ord + h - j - 1][abs + i].graysc,
					sizeof(img->imga.data[ord + h - j - 1][abs + i].graysc));
				} else {
					memmove(&aux[i][j].rgb,
							&img->imga.data[ord + h - j - 1][abs + i].rgb,
					sizeof(img->imga.data[ord + h - j - 1][abs + i].rgb));
				}
			}
		}
		copy_matrix_ascii(img, aux, selected_zone, status);
	} else if (strcmp(status, "binary") == 0) {
		pixel_type_bin **aux = NULL;
		aux = create_bin(h, w);
		for (int i = 0; i < w; i++) {
			for (int j = 0; j < h; j++) {
				if (img->imgb.magicword[1] != '6') {
					memmove(&aux[i][j].graysc,
							&img->imgb.data[ord + h - j - 1][abs + i].graysc,
					sizeof(img->imgb.data[ord + h - j - 1][abs + i].graysc));
				} else {
					memmove(&aux[i][j].rgb,
							&img->imgb.data[ord + h - j - 1][abs + i].rgb,
					sizeof(img->imgb.data[ord + h - j - 1][abs + i].rgb));
				}
			}
		}
		copy_matrix_bin(img, aux, selected_zone, status);
	}
}

void cnt_rotate(img_type *img, char status[], zone *selected_zone, int cnt)
{
	for (int i = 0; i < cnt; i++)
		rotate_90(img, status, selected_zone);
}

/*stabilim tipul fisierului si incarcam metadatele si matricea de pixeli in
memorie dupa deschirea fisierului*/
void read_load(img_type *img, zone *selected_zone, char filename[],
			   char status[])
{
	FILE *f;
	scanf("%s", filename);
	f = fopen(filename, "rt");
	if (!f) {
		printf("Failed to load %s\n", filename);
		if (strcmp(status, "empty"))
			exit_all(status, img, 0);
		strcpy(status, "empty");
	} else {
		if (strcmp(status, "empty"))
			exit_all(status, img, 0);
		char magic_word[3];
		skip_comm(f);
		fscanf(f, "%s", magic_word);
		is_binary(&f, magic_word, filename, status);
		load(f, img, magic_word, filename);
		select_all(status, img, selected_zone, 0);
	}
}

/*verificam daca datele de intrare sunt valide pentru comanda SAVE*/
void read_save(img_type *img, char filename[], char save_type[], char status[])
{
	char separator;
	scanf("%s", filename);
	scanf("%c", &separator);
	if (separator == '\n')
		strcpy(save_type, "binary");
	else
		if (separator == ' ') {
			scanf("%c", &separator);
			if (separator == '\n') {
				strcpy(save_type, "binary");
			} else {
				if (separator == 'a') {
					scanf("%s", save_type + 1);
					save_type[0] = 'a';
					if (strcmp(save_type, "ascii") != 0) {
						printf("Invalid command\n");
						return;
					}
				}
			}
		}
	save(img, filename, status, save_type);
}

/*citim datele de intrare si verificam daca sunt valide pentru comenzi*/
void read_command(char command[], char status[])
{
	img_type img;
	zone selected_zone;
	char filename[MAX_LENGHT], save_type[MAX_LENGHT], inv_cmd;

	while (scanf("%s", command)) {
		if (strcmp(command, "LOAD") == 0) {
			read_load(&img, &selected_zone, filename, status);
		} else if (strcmp(command, "SELECT") == 0) {
			coordinates(&img, &selected_zone, status);
		} else if (strcmp(command, "CROP") == 0) {
			crop(status, &img, &selected_zone);
		} else if (strcmp(command, "APPLY") == 0) {
			apply(status, &img, selected_zone);
		} else if (strcmp(command, "HISTOGRAM") == 0) {
			histogram(&img, status);
		} else if (strcmp(command, "EQUALIZE") == 0) {
			equalize_graysc(status, &img);
		} else if (strcmp(command, "SAVE") == 0) {
			read_save(&img, filename, save_type, status);
		} else if (strcmp(command, "EXIT") == 0) {
			exit_all(status, &img, 1);
			break;
		} else if (strcmp(command, "ROTATE") == 0) {
			rotate(&img, status, &selected_zone);
		} else {
			scanf("%c", &inv_cmd);
			while (inv_cmd != '\n' && inv_cmd != EOF)
				scanf("%c", &inv_cmd);
			printf("Invalid command\n");
		}
	}
}

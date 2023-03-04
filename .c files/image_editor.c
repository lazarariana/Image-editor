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
#define MAX_LENGHT 15

int main(void)
{
	char command[MAX_LENGHT], status[MAX_LENGHT];
	strcpy(status, "empty");
	read_command(command, status);

	return 0;
}

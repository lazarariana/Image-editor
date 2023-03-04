# compiler setup
CC=gcc
CFLAGS=-Wall -Wextra -std=c99

# define targets
TARGETS=image_editor

build: $(TARGETS)

image_editor: image_editor.c
	$(CC) $(CFLAGS) image_editor.c commands.c edit_functions.c -o image_editor -lm


pack:
	zip -FSr 312CA_Lazar-AndreiAriana-Maria_Tema3.zip *.c *.h Makefile README

clean:
	rm -f $(TARGETS)

.PHONY: pack clean

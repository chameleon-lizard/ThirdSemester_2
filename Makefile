all: prac

WARNINGS = -Wall
DEBUG = -ggdb -fno-omit-frame-pointer
OPTIMIZE = -O2

prac: Makefile 1_prac.c
	gcc -o prac -Wall 1_prac.c

clean:
	rm prac


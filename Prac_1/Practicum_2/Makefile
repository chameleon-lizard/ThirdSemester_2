all: practicum_2

WARNINGS = -Wall
DEBUG = -ggdb -fno-omit-frame-pointer
OPTIMIZE = -O2

practicum_2: Makefile practicum_2.c
	$(CC) -o $@ $(WARNINGS) $(DEBUG) $(OPTIMIZE) practicum_2.c
	cat lol > file

clean:
	rm -f practicum_2

debug:
	gdb --args ./practicum_2 file

run:
	./practicum_2 file

all: practicum_1

WARNINGS = -Wall
DEBUG = -ggdb -fno-omit-frame-pointer
OPTIMIZE = -O2

practicum_1: Makefile practicum_1.c
	$(CC) -o $@ $(WARNINGS) $(DEBUG) $(OPTIMIZE) practicum_1.c

clean:
	rm -f practicum_1

debug:
	gdb ./practicum_1

run:
	./practicum_1

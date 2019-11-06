all: practicum

WARNINGS = -Wall
DEBUG = -ggdb -fno-omit-frame-pointer --coverage
OPTIMIZE = -O2

practicum: Makefile practicum.c
	$(CC) -o $@ $(WARNINGS) $(DEBUG) $(OPTIMIZE) practicum.c

clean:
	rm -f practicum

debug:
	gdb --args ./practicum ls -la lol.txt ls wc -c 

run:
	./practicum ls -la lol.txt ls wc -c

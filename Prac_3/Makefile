all: practicum

WARNINGS = -Wall
DEBUG = -ggdb -fno-omit-frame-pointer --coverage
OPTIMIZE = -O2

practicum: Makefile practicum.c
	$(CC) -o $@ $(WARNINGS) $(DEBUG) $(OPTIMIZE) practicum.c

clean:
	rm -f practicum
	rm -f practicum.gcda
	rm -f practicum.gcno

debug:
	gdb --command commands --args ./practicum 5 3

run:
	./practicum 5 3

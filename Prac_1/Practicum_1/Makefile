TARGET = practicum_1

WARNINGS = -Wall
DEBUG = -ggdb -fno-omit-frame-pointer
OPTIMIZE = -O2

$(TARGET): main.o lib.a
	    gcc $^ $(WARNINGS) $(DEBUG) $(OPTIMIZE) -o practicum_1

main.o: practicum_1.c
	    gcc -c $< -o $@

lib.a: list.o
	    ar rcs $@ $^

list.o: list.c list.h
	    gcc -c -o $@ $<

clean:
	    rm -f *.o *.a $(TARGET)

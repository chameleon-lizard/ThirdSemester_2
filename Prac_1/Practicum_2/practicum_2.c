#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>

enum
{
    STRING_SIZE = 80
};

int
main (int argc, char *argv[])
{
    if (argc < 2) {
        printf("No file specified\n");
        return 1;
    }

    FILE *file = fopen(argv[1], "r+");

    if (file == NULL) {
        printf("Problem opening file.\n");
        return 1;
    }

    char string[STRING_SIZE + 1] = { 0 };
    char buffer[STRING_SIZE + 1] = { 0 };

    while (fgets(string, STRING_SIZE + 1, file) != NULL) {
        int length = strlen(string);
        memset(buffer, 0, STRING_SIZE + 1);
        for (int i = 1; length - i >= 0; i++) {
            buffer[i - 1] = string[length - i];
        }

        fseek(file, -length, SEEK_CUR);
    	fputs(&buffer[1], file);
        fseek(file, 1, SEEK_CUR);
    }

    fclose(file);

    return 0;
}

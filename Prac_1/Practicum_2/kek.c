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

    char *string = (char *)calloc(STRING_SIZE + 1, sizeof(*string));
    char *buffer = (char *)calloc(STRING_SIZE + 1, sizeof(*buffer));

    int lol = 0;
    while ((lol = fscanf(file, "%80s", string)) != EOF) {
        int length = strlen(string);
        memset(buffer, 0, STRING_SIZE + 1);
        for (int i = 1; length - i >= 0; i++) {
            buffer[i - 1] = string[length - i];
        }

        fseek(file, -length, SEEK_CUR);
    	fputs(buffer, file);
    }

    fclose(file);

    return 0;
}

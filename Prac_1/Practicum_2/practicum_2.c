#include <string.h>
#include <stdio.h>

enum
{
    STRING_SIZE = 80
};

int
main (int argc, char *argv[])
{
    // Checking if we actually have the file n the arguments:
    if (argc < 2) {
        printf("No file specified\n");
        return 1;
    }

    // Opening the file, checking if it has been opened:
    FILE *file = fopen(argv[1], "r+");
    if (file == NULL) {
        printf("Problem opening file.\n");
        return 1;
    }

    // Creating string and the buffer for the readed 80-symbol strings:
    char string[STRING_SIZE + 1] = { 0 };
    char buffer[STRING_SIZE + 1] = { 0 };

    // While we can read, we are reading:
    while (fgets(string, STRING_SIZE + 1, file) != NULL) {
        // Length of the string can be less then 80:
        int length = strlen(string);
        memset(buffer, 0, STRING_SIZE + 1);

        // Changing the items in strings:
        for (int i = 1; length - i >= 0; i++) {
            buffer[i - 1] = string[length - i];
        }

        // Writing to the file, changing the pointer to the current place in
        // the file to the beginning of the string and then to the end of
        // the string:
        fseek(file, -length, SEEK_CUR);
    	fputs(&buffer[1], file);
        fseek(file, 1, SEEK_CUR);
    }

    // Closing the file:
    fclose(file);

    return 0;
}

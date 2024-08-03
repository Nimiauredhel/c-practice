#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *strArr[] =
{
    "first", "second", "third"
};

char strCount = sizeof(strArr)/sizeof(strArr[0]);

int main(int argc, char** argv)
{
    // receive target path from command line,
    // else use default target path.
    // write a given array to a binary file,
    // then read it back and output to stdout.

    char *path = malloc(sizeof(char)*13);
    strcpy(path, "./output.bin");

    FILE *outFile = fopen(path, "w");
    size_t written =
    fwrite(strArr, sizeof(strArr[0]), strCount, outFile);
    printf("Number of elements written: %zu\n", written);
    fclose(outFile);

    char **newStrArr = malloc(sizeof(strArr));
    FILE *inFile = fopen(path, "r");
    size_t read =
    fread(newStrArr, sizeof(strArr[0]), strCount, inFile); 
    printf("Number of elements read: %zu\n", read);

    printf("Read file elements:\n");

    for (int i = 0; i < strCount; i++)
    {
        printf("%s\n", newStrArr[i]);
    }

    printf("Program finished. Thank you.\n");

}

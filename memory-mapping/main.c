#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>

#define MAPPED_LENGTH 64
const char path[] = "./mapped.txt";

int file_descriptor;
int file_position = 0;
char input;
char *mapped_file;

void initialize_map(void)
{
    char empty_file[MAPPED_LENGTH];
    memset(empty_file, ' ', MAPPED_LENGTH);
    file_descriptor = open(path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    write(file_descriptor, empty_file, MAPPED_LENGTH);
    mapped_file = mmap(NULL, MAPPED_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, file_descriptor, 0);
}

int main(void)
{
    initialize_map();

    printf("Input changes to file. When done, input EOF (Ctrl-D).\n");

    input = getchar();

    while (input != EOF)
    {
        if (file_position >= MAPPED_LENGTH) file_position = 0;
        mapped_file[file_position] = input;
        file_position++;
        input = fgetc(stdin);
    }

    printf("Done changing file.\n");
}

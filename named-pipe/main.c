#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>

int main(void)
{
    int fd = open("./pipe", O_RDONLY, S_IWUSR);
    char c = ' ';

    printf("Reading from named pipe until termination.\n");

    for(;;)
    {
        if (read(fd, &c, 1) > 0)
        {
            printf("%c", c);
        }
    }
}

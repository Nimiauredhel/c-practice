#include <stdlib.h>
#include <stdio.h>

int main(void)
{
    char input[25];
    unsigned int length;
    unsigned int height;

    printf("\nHow tall do you want that pyramid?\n\r");

    fgets(input, 4, stdin);

    height = atoi(input);
    length = height*2;
    unsigned char level[length+1];
    level[length] = 0;

    for(int i = 0; i < length; i++)
    {
        level[i] = '*';
    }

    printf("\n");
    unsigned int currentLength = length;

    while(currentLength > height)
    {
        printf("%s\n", level);
        level[length - currentLength] = ' ';
        level[currentLength-1] = ' ';
        currentLength--;
    }

    printf("\nDoes this look right?\n");

    return 0;
}

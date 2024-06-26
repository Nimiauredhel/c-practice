#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct myStruct
{
    char groupA[4][4];
    char groupB[4][4];
    char groupC[4][4];
};

int main(void)
{
    char input[25];
    int index;
    struct myStruct mystr;

    for(int i = 0; i < 4; i++)
    {
        char str[4];
        snprintf(str, 4, "A:%d\0", i);
        strncpy(mystr.groupA[i], str, 4);
        snprintf(str, 4, "B:%d\0", i);
        strncpy(mystr.groupB[i], str, 4);
        snprintf(str, 4, "C:%d\0", i);
        strncpy(mystr.groupC[i], str, 4);
    }

    printf("\n|");
    for(int i = 0; i < 4; i++)
    {
        printf("%s|", mystr.groupA[i]);
    }
    printf("\n|");
    for(int i = 0; i < 4; i++)
    {
        printf("%s|", mystr.groupB[i]);
    }
    printf("\n|");
    for(int i = 0; i < 4; i++)
    {
        printf("%s|", mystr.groupC[i]);
    }

    printf("\n\nHi.\nWhat is your name?\n\r");

    fgets(input, 25, stdin);
    input[strlen(input)-1] = '\0';

    printf("Welcome %s! Have a knife day.\nI'm trying to decide which cell to print from the B array.\nWould you kindly pick a number between 0 and 3?\n", input);

    fgets(input, 4, stdin);

    printf("\nThank you very much. Let's see how that went...\n\n");

    index = atoi(input);

    printf("The value for that index is: %s.\nHow does that make you feel?\n", mystr.groupB[index]);

    return 0;
}

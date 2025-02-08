#include <stdio.h>
#include "src/add.h"
#include "src/mult.h"

int main(void)
{
    printf("Good add: %d+%d=%d\n", 5, 13, add(5, 13));
    printf("Bad add: %d+%d=%d\n", 5, 13, bad_add(5, 13));

    printf("Good mult: %dx%d=%d\n", 5, 10, mult(5, 10));
    printf("Bad mult: %dx%d=%d\n", 5, 10, bad_mult(5, 10));
}

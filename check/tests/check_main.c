#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include "check_add.c"
#include "check_mult.c"

int main(void)
{
    int number_failed = 0;
    Suite *s;
    SRunner *sr;

    s = add_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed += srunner_ntests_failed(sr);
    srunner_free(sr);

    s = mult_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed += srunner_ntests_failed(sr);
    srunner_free(sr);

    printf("Total failed: %d\n", number_failed);

    return number_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

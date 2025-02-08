#include <check.h>
#include "../src/mult.h"

START_TEST(test_mult)
{
    int a = 5;
    int b = 10;
    ck_assert_int_eq(mult(5, 10), 50);
}
END_TEST

START_TEST(test_bad_mult)
{
    int a = 5;
    int b = 10;
    ck_assert_int_eq(bad_mult(5, 10), 50);
}
END_TEST

Suite * mult_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Mult");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_mult);
    tcase_add_test(tc_core, test_bad_mult);
    suite_add_tcase(s, tc_core);

    return s;
}

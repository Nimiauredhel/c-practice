#include <check.h>
#include "../src/add.h"

START_TEST(test_add)
{
    int a = 5;
    int b = 13;
    ck_assert_int_eq(add(5, 13), 18);
}
END_TEST

START_TEST(test_bad_add)
{
    int a = 5;
    int b = 13;
    ck_assert_int_eq(bad_add(5, 13), 18);
}
END_TEST

Suite * add_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Add");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_add);
    tcase_add_test(tc_core, test_bad_add);
    suite_add_tcase(s, tc_core);

    return s;
}

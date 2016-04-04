#include <stddef.h>
#include <stdarg.h>
#include <setjmp.h>
#include <cmocka.h>
#include "fod_common.h"

extern void test_common_substring_to_uint_1(void **st) {
    char const *str = "12345678901234567890123456";
    fod_longest_uint res = 0;
    int ok;
    
    ok = fod_substring_to_uint(&res, &str[0], &str[2]);
    assert_true(ok);
    assert_true(res == 12);
}

extern void test_common_substring_duplicate(void **st) {

    char const *str = "'1\\'' - \"3\\\"'\\n4\"";
    //                 '1\''     "3\\"'\n4"

    const char * res;

    res = fod_substring_duplicate_and_unquote(&str[0], &str[5],
					          fod_std_realloc, NULL);
    assert_non_null(res);
    assert_string_equal(res, "1'");
}

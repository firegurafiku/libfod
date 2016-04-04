#include <stdlib.h>
#include <setjmp.h>
#include <stdarg.h>
#include <cmocka.h>
#include "generated_tests.h"

int main(void) {
    int retcode;
    size_t i, n;

    n = sizeof(global_test_groups)/sizeof(global_test_groups[0]);

    retcode = EXIT_SUCCESS;
    for (i=0; i<n; ++i) {
        struct TestGroup const* group = &global_test_groups[i];
        retcode = _cmocka_run_group_tests("",
                                         group->tests,
                                         group->tests_count,
                                         group->setup,
                                         group->teardown);
    }

    return retcode;
}

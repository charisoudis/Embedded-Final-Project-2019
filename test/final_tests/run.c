#include "test_utils.h"

int main(void) {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(test_explode),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}

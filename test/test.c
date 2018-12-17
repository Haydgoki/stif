#include <check.h>
#include <stdlib.h>
#include "../include/stif.h"

START_TEST(read_stif_block_buffer_null){
    stif_block_t *block = NULL;
    size_t size = 20;
    block = read_stif_block(NULL, 10, &size);
    ck_assert(block == NULL);
}
END_TEST

static Suite *parse_suite(void)
{
    Suite *s;
    TCase *tc_read_stif;
    s = suite_create("stif");
    tc_read_stif = tcase_create("read_stif");
    tcase_add_test(tc_read_stif, read_stif_block_buffer_null);
    suite_add_tcase(s, tc_read_stif);

    return s;
}

int main(void) {
    int no_failed = 0;
    Suite *s;
    SRunner *runner;

    s = parse_suite();
    runner = srunner_create(s);

    srunner_run_all(runner, CK_NORMAL);
    no_failed = srunner_ntests_failed(runner);
    srunner_free(runner);
    return (no_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

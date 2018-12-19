#include <check.h>
#include <stdlib.h>
#include "../include/stif.h"


/***************************************************************

                    _btoi_test

***************************************************************/

START_TEST(_btoi_test_good_number){
    unsigned char b[] = "\x05\x00\x00\x00";
    int32_t res = _btoi(b);
    ck_assert(res == 5);
}
END_TEST

/***************************************************************

                    read_stif_block

***************************************************************/

START_TEST(read_stif_block_buffer_null){
    stif_block_t *block = NULL;
    size_t size = 20;
    block = read_stif_block(NULL, 10, &size);
    ck_assert(block == NULL);
}
END_TEST

START_TEST(read_stif_block_size){
    stif_block_t *block = NULL;
    unsigned char buffer[] = "\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
    size_t size = 20;
    block = read_stif_block(buffer, 0, &size);
    ck_assert(block == NULL);
    block = read_stif_block(buffer, 4, &size);
    ck_assert(block == NULL);
    block = read_stif_block(buffer, 5, &size);
    ck_assert(block != NULL);
}
END_TEST

START_TEST(read_stif_block_is_data){
    stif_block_t *block = NULL;
    size_t size = 20;
    unsigned char buffer[] = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
    block = read_stif_block(buffer, 20, &size);
    ck_assert(block == NULL);

    unsigned char buffer2[] = "\x0A\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
    block = read_stif_block(buffer2, 20, &size);
    ck_assert(block == NULL);

    unsigned char buffer3[] = "\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
    block = read_stif_block(buffer3, 20, &size);
    ck_assert(block != NULL);
}
END_TEST

START_TEST(read_stif_block_bytes_read){
    // Tester si dans l'entête la taille correspond aux bytes read + 5
    stif_block_t *block = NULL;
    size_t size = 20;                            //datas
    unsigned char buffer[] = "\x01\x04\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
    block = read_stif_block(buffer, 10, &size);
    ck_assert(size == 9);
    ck_assert(block != NULL);
    unsigned char buffer2[] = "\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
    block = read_stif_block(buffer2, 10, &size);
    ck_assert(size == 5);
    ck_assert(block != NULL);
    // Negative size
    unsigned char buffer3[] = "\x01\x04\x00\x00\x80\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
    block = read_stif_block(buffer3, 10, &size);
    ck_assert(size == 5);
    ck_assert(block == NULL);



}
END_TEST


START_TEST(read_stif_data_not_null){
    // Tester si dans l'entête la taille correspond aux bytes read + 5
    stif_block_t *block = NULL;
    size_t size = 20;                            //datas
    unsigned char buffer[] = "\x01\x04\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
    block = read_stif_block(buffer, 10, &size);
    ck_assert(size == 9);
    ck_assert(block != NULL);
    ck_assert(block -> data != NULL);
}
END_TEST

/***************************************************************

                    parse_stif

***************************************************************/

START_TEST(stif_parse_too_little){
    unsigned char buffer[] = "\x01";
    stif_t * stif = parse_stif(buffer, 1);
    ck_assert(stif==NULL);
}
END_TEST

START_TEST(stif_parse_bad_magic){
    unsigned char buffer[] = "\xfe\xaa\x00\x09\x00\x00\x00\x01\x00\x00\x00\x01\x00\x00\x00\x00\x01\x01\x00\x00\x00\xFF"; //Image basique avec bad magic
    stif_t * stif = parse_stif(buffer, 22);
    ck_assert(stif == NULL);
    unsigned char buffer2[] = "\xfe\xca\x00\x09\x00\x00\x00\x01\x00\x00\x00\x01\x00\x00\x00\x00\x01\x01\x00\x00\x00\xFF"; //Image basique 1 pixel noir
    stif = parse_stif(buffer2, 22);
    ck_assert(stif != NULL);
}
END_TEST


START_TEST(stif_parse_first_block_header){
    unsigned char buffer[] = "\xfe\xca\x01\x09\x00\x00\x00\x01\x00\x00\x00\x01\x00\x00\x00\x00\x01\x01\x00\x00\x00\xFF"; //Image basique 1 pixel noir
    stif_t * stif = parse_stif(buffer, 22);
    ck_assert(stif == NULL);
    unsigned char buffer2[] = "\xfe\xca\x00\x09\x00\x00\x00\x01\x00\x00\x00\x01\x00\x00\x00\x00\x01\x01\x00\x00\x00\xFF"; //Image basique 1 pixel noir
    stif = parse_stif(buffer2, 22);
    ck_assert(stif != NULL);
}
END_TEST


START_TEST(stif_parse_bad_header_size){
    unsigned char buffer[] = "\xfe\xca\x00\x08\x00\x00\x00\x01\x00\x00\x00\x01\x00\x00\x00\x00\x01\x01\x00\x00\x00\xFF"; //Image basique 1 pixel noir
    stif_t * stif = parse_stif(buffer, 22);
    ck_assert(stif == NULL);
    unsigned char buffer2[] = "\xfe\xca\x00\x00\x00\x00\x00\x01\x00\x00\x00\x01\x00\x00\x00\x00\x01\x01\x00\x00\x00\xFF"; //Image basique 1 pixel noir
    stif = parse_stif(buffer2, 22);
    ck_assert(stif == NULL);
    unsigned char buffer3[] = "\xfe\xca\x00\x09\x00\x00\x00\x01\x00\x00\x00\x01\x00\x00\x00\x00\x01\x01\x00\x00\x00\xFF"; //Image basique 1 pixel noir
    stif = parse_stif(buffer3, 22);
    ck_assert(stif != NULL);
}
END_TEST


static Suite *parse_suite(void)
{
    Suite *s;
    TCase *tc_btoi;
    TCase *tc_read_stif_block;
    TCase *tc_parse_stif;
    s = suite_create("stif");

    /*_btoi tests*/
    tc_btoi = tcase_create("btoi");
    tcase_add_test(tc_btoi, _btoi_test_good_number);

    /*read_stif_block tests*/
    tc_read_stif_block = tcase_create("read_stif");
    tcase_add_test(tc_read_stif_block, read_stif_block_buffer_null);
    tcase_add_test(tc_read_stif_block, read_stif_block_size);
    tcase_add_test(tc_read_stif_block, read_stif_block_is_data);
    tcase_add_test(tc_read_stif_block, read_stif_block_bytes_read);
    tcase_add_test(tc_read_stif_block, read_stif_data_not_null);


    /*parse_stif tests*/
    tc_parse_stif = tcase_create("parse_stif");
    tcase_add_test(tc_parse_stif, stif_parse_too_little);
    tcase_add_test(tc_parse_stif, stif_parse_bad_magic);
    tcase_add_test(tc_parse_stif, stif_parse_first_block_header);
    tcase_add_test(tc_parse_stif, stif_parse_bad_header_size);



    /*Add all tcase tests*/
    suite_add_tcase(s, tc_btoi);
    suite_add_tcase(s, tc_read_stif_block);
    suite_add_tcase(s, tc_parse_stif);
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

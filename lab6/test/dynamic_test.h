#ifndef _DYNAMIC_TEST_H_
#define _DYNAMIC_TEST_H_
void test_dynamic_free_prev_merge();
void test_dynamic_free_next_merge();
void test_dynamic_free_both_merge();
void test_dynamic_free_top_chunk_merge();
void test_dynamic_free_merge_to_unsorted_bin();
void test_dynamic_malloc_size();
void test_dynamic_find_free_chunk();
void test_dynamic_unsorted_bin_malloc_split_to_bin_list();
void test_dynamic_unsorted_bin_malloc_split_to_unsorted_bin();
void test_dynamic_request_new_page();
void test_dynamic_main();
#endif
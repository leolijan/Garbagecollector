#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash_table-dynamic.h"
#include <CUnit/Basic.h>
#include "linked_list.h"
#include "common.h"
#include "../../src/gc.h"

heap_t *heap = NULL; /// Added

int init_suite(void) {
  // Change this function if you want to do something *before* you
  // run a test suite
  size_t bytes = 2048 * 2;
  bool unsafe_stack = false;
  float gc_threshold = 0.5;
  heap_t *heap = h_init(bytes, unsafe_stack, gc_threshold); /// Added

  if (heap == NULL) {
	return -1;
  }
  return 0;
}

int clean_suite(void) {
  // Change this function if you want to do something *after* you
  // run a test suite
  return 0;
}


void test_create_destroy() {
	ioopm_hash_table_t *ht = ioopm_hash_table_create(hash_int,compare_int,compare_string, heap);
	CU_ASSERT_PTR_NOT_NULL(ht);
}

void test_insert_once() {
	ioopm_hash_table_t *ht = ioopm_hash_table_create(hash_int,compare_int,compare_string, heap);
	ioopm_elem_t k = (ioopm_elem_t)800;
	char *v = "test";
	
	//verify that key is not in hash table
	CU_ASSERT_PTR_NULL(ioopm_hash_table_lookup(ht,k).string);

	//insert value with key into hash table
	ioopm_hash_table_insert(ht,k,(ioopm_elem_t)v, heap);
		
	//verify that the value has been inserted
	CU_ASSERT_STRING_EQUAL(ioopm_hash_table_lookup(ht,k).string,v);
}

void test_lookup_empty() {
	ioopm_hash_table_t *ht = ioopm_hash_table_create(hash_int,compare_int,compare_string, heap);
   
   ///Loop through the hashtable with size 17 (?)
   for (int i = 0; i < No_Buckets; ++i) {
       CU_ASSERT_PTR_NULL(ioopm_hash_table_lookup(ht, (ioopm_elem_t)i).string);
     }   
   
   CU_ASSERT_PTR_NULL(ioopm_hash_table_lookup(ht, (ioopm_elem_t)-1).string);
}

void test_remove_once() {
	ioopm_hash_table_t *ht = ioopm_hash_table_create(hash_int,compare_int,compare_string, heap);
	ioopm_elem_t k = (ioopm_elem_t)800;
	char *v = "test";

	//verify that key is not in hash table
	CU_ASSERT_PTR_NULL(ioopm_hash_table_lookup(ht,k).string);

	//insert value with key into hash table
	ioopm_hash_table_insert(ht,k,(ioopm_elem_t)v, heap);

	//verify that the value has been inserted
	CU_ASSERT_STRING_EQUAL(ioopm_hash_table_lookup(ht,k).string,v);
	
	ioopm_hash_table_remove(ht,k);

	//verify that key is not in hash table
	CU_ASSERT_PTR_NULL(ioopm_hash_table_lookup(ht,k).string);
}

void test_remove_multiple() {
	ioopm_hash_table_t *ht = ioopm_hash_table_create(hash_int,compare_int,compare_string, heap);

	ioopm_hash_table_insert(ht,(ioopm_elem_t)1,(ioopm_elem_t)"hej", heap);
	ioopm_hash_table_insert(ht,(ioopm_elem_t)18,(ioopm_elem_t)"va", heap);
	ioopm_hash_table_insert(ht,(ioopm_elem_t)35,(ioopm_elem_t)"hejsan", heap);

	ioopm_hash_table_remove(ht,(ioopm_elem_t)18);
	
	CU_ASSERT_PTR_NULL(ioopm_hash_table_lookup(ht,(ioopm_elem_t)18).string);
	CU_ASSERT_PTR_NOT_NULL(ioopm_hash_table_lookup(ht,(ioopm_elem_t)1).string);
	CU_ASSERT_PTR_NOT_NULL(ioopm_hash_table_lookup(ht,(ioopm_elem_t)35).string);	
	
}

void test_empty_hash_table() {
	ioopm_hash_table_t *ht = ioopm_hash_table_create(hash_int,compare_int,compare_string, heap);
	CU_ASSERT_TRUE(ioopm_hash_table_is_empty(ht));
	
	ioopm_hash_table_insert(ht,(ioopm_elem_t)10,(ioopm_elem_t)"hej", heap);
	ioopm_hash_table_insert(ht,(ioopm_elem_t)11,(ioopm_elem_t)"hejsan", heap);
	
	CU_ASSERT_FALSE(ioopm_hash_table_is_empty(ht));
}

void test_count_size() {
	ioopm_hash_table_t *ht = ioopm_hash_table_create(hash_int,compare_int,compare_string, heap);
	ioopm_hash_table_insert(ht,(ioopm_elem_t)10,(ioopm_elem_t)"hej", heap);
	ioopm_hash_table_insert(ht,(ioopm_elem_t)11,(ioopm_elem_t)"hejsan", heap);
	
	CU_ASSERT_EQUAL(ioopm_hash_table_size(ht),2);
	
	ioopm_hash_table_insert(ht,(ioopm_elem_t)12,(ioopm_elem_t)"tjena", heap);
	
	CU_ASSERT_EQUAL(ioopm_hash_table_size(ht),3);
	
	ioopm_hash_table_remove(ht,(ioopm_elem_t)11);
	ioopm_hash_table_remove(ht,(ioopm_elem_t)10);
	
	CU_ASSERT_EQUAL(ioopm_hash_table_size(ht),1);
	
}

void test_clear() {
	ioopm_hash_table_t *ht = ioopm_hash_table_create(hash_int,compare_int,compare_string, heap);
	ioopm_hash_table_insert(ht,(ioopm_elem_t)10,(ioopm_elem_t)"hej", heap);
	ioopm_hash_table_insert(ht,(ioopm_elem_t)11,(ioopm_elem_t)"hejsan", heap);
	ioopm_hash_table_clear(ht);
	
	CU_ASSERT_TRUE(ioopm_hash_table_is_empty(ht));

	ioopm_hash_table_insert(ht,(ioopm_elem_t)11,(ioopm_elem_t)"hejsan", heap);
	CU_ASSERT_FALSE(ioopm_hash_table_is_empty(ht));
	
}

void test_values() {
	ioopm_hash_table_t *ht = ioopm_hash_table_create(hash_int,compare_int,compare_string, heap);
	ioopm_hash_table_insert(ht,(ioopm_elem_t)10,(ioopm_elem_t)"hej", heap);
	ioopm_hash_table_insert(ht,(ioopm_elem_t)11,(ioopm_elem_t)"hejsan", heap);
	ioopm_hash_table_insert(ht,(ioopm_elem_t)12,(ioopm_elem_t)"tjena", heap);
	
	ioopm_list_t *values = ioopm_hash_table_values(ht, heap);

	CU_ASSERT_TRUE(ioopm_linked_list_contains(values,(ioopm_elem_t)"hej"));
	CU_ASSERT_TRUE(ioopm_linked_list_contains(values,(ioopm_elem_t)"hejsan"));
	CU_ASSERT_TRUE(ioopm_linked_list_contains(values,(ioopm_elem_t)"tjena"));
<<<<<<< HEAD
	
=======
>>>>>>> footer
}

void test_keys() {
	ioopm_hash_table_t *ht = ioopm_hash_table_create(hash_int,compare_int,compare_string, heap);
	ioopm_hash_table_insert(ht,(ioopm_elem_t)10,(ioopm_elem_t)"hej", heap);
	ioopm_hash_table_insert(ht,(ioopm_elem_t)11,(ioopm_elem_t)"hejsan", heap);
	ioopm_hash_table_insert(ht,(ioopm_elem_t)12,(ioopm_elem_t)"tjena", heap);

	//int true_keys[3] = {10, 11, 12};
	ioopm_list_t *keys = ioopm_hash_table_keys(ht, heap);

	CU_ASSERT_TRUE(ioopm_linked_list_contains(keys,(ioopm_elem_t)10));
	CU_ASSERT_TRUE(ioopm_linked_list_contains(keys,(ioopm_elem_t)11));
	CU_ASSERT_TRUE(ioopm_linked_list_contains(keys,(ioopm_elem_t)12));

}

void test_has_value() {
	ioopm_hash_table_t *ht = ioopm_hash_table_create(hash_int,compare_int,compare_string, heap);
	ioopm_hash_table_insert(ht,(ioopm_elem_t)10,(ioopm_elem_t)"hej", heap);
	ioopm_hash_table_insert(ht,(ioopm_elem_t)11,(ioopm_elem_t)"hejsan", heap);
	ioopm_hash_table_insert(ht,(ioopm_elem_t)12,(ioopm_elem_t)"tjena", heap);

	CU_ASSERT_TRUE(ioopm_hash_table_has_value(ht,(ioopm_elem_t)"hejsan"));
	CU_ASSERT_FALSE(ioopm_hash_table_has_value(ht,(ioopm_elem_t)"va"));

}

void test_has_key() {
	ioopm_hash_table_t *ht = ioopm_hash_table_create(hash_int,compare_int,compare_string, heap);
	ioopm_hash_table_insert(ht,(ioopm_elem_t)10,(ioopm_elem_t)"hej", heap);
	ioopm_hash_table_insert(ht,(ioopm_elem_t)11,(ioopm_elem_t)"hejsan", heap);
	ioopm_hash_table_insert(ht,(ioopm_elem_t)12,(ioopm_elem_t)"tjena", heap);

	CU_ASSERT_TRUE(ioopm_hash_table_has_key(ht,(ioopm_elem_t)10));
	CU_ASSERT_FALSE(ioopm_hash_table_has_key(ht,(ioopm_elem_t)99));
	

}

void test_has_value_all() {
	ioopm_hash_table_t *ht = ioopm_hash_table_create(hash_int,compare_int,compare_string, heap);
	ioopm_hash_table_insert(ht,(ioopm_elem_t)10,(ioopm_elem_t)"hej", heap);
	ioopm_hash_table_insert(ht,(ioopm_elem_t)11,(ioopm_elem_t)"hej", heap);
	ioopm_hash_table_insert(ht,(ioopm_elem_t)12,(ioopm_elem_t)"hej", heap);

	CU_ASSERT_TRUE(ioopm_hash_table_all(ht,value_equiv,"hej"));
	ioopm_hash_table_insert(ht,(ioopm_elem_t)12,(ioopm_elem_t)"hejsan", heap);
	CU_ASSERT_FALSE(ioopm_hash_table_all(ht,value_equiv,"hej"));

	
}

//apply function
static void change_values(ioopm_elem_t key,ioopm_elem_t *value, void *x) {
	ioopm_elem_t *other_value_ptr = x;
	*value = *other_value_ptr;
}

void test_apply_all() {
	ioopm_hash_table_t *ht = ioopm_hash_table_create(hash_int,compare_int,compare_string, heap);
	ioopm_hash_table_insert(ht,(ioopm_elem_t)10,(ioopm_elem_t)"hej", heap);
	ioopm_hash_table_insert(ht,(ioopm_elem_t)11,(ioopm_elem_t)"hej", heap);
	ioopm_hash_table_insert(ht,(ioopm_elem_t)12,(ioopm_elem_t)"hej", heap);

	ioopm_hash_table_apply_to_all(ht,change_values,"hejsan");
	CU_ASSERT_TRUE(ioopm_hash_table_all(ht,value_equiv,"hejsan"));


}

int main() {
  // First we try to set up CUnit, and exit if we fail
  if (CU_initialize_registry() != CUE_SUCCESS)
    return CU_get_error();

  // We then create an empty test suite and specify the name and
  // the init and cleanup functions
  CU_pSuite my_test_suite = CU_add_suite("My awesome test suite", init_suite, clean_suite);
  if (my_test_suite == NULL) {
      // If the test suite could not be added, tear down CUnit and exit
      CU_cleanup_registry();
      return CU_get_error();
  }

  // This is where we add the test functions to our test suite.
  // For each call to CU_add_test we specify the test suite, the
  // name or description of the test, and the function that runs
  // the test in question. If you want to add another test, just
  // copy a line below and change the information
  if (
    (CU_add_test(my_test_suite, "Creat/Destroy hash table", test_create_destroy) == NULL) ||
	(CU_add_test(my_test_suite, "Lookup in hashtable", test_lookup_empty) == NULL) ||
	(CU_add_test(my_test_suite, "Insert one value", test_insert_once) == NULL) ||
	(CU_add_test(my_test_suite, "Remove one value", test_remove_once) == NULL) ||
	(CU_add_test(my_test_suite, "Is hash table empty", test_empty_hash_table) == NULL) ||
	(CU_add_test(my_test_suite, "Count the hash table size", test_count_size) == NULL) ||
	(CU_add_test(my_test_suite, "Clear the hash_table", test_clear) == NULL) ||
	(CU_add_test(my_test_suite, "Test if all values match", test_values) == NULL) ||
	(CU_add_test(my_test_suite, "Test if all keys match", test_keys) == NULL) ||
	(CU_add_test(my_test_suite, "Test if hash table has value", test_has_value) == NULL) ||
	(CU_add_test(my_test_suite, "Test if hash table has key", test_has_key) == NULL) ||
	(CU_add_test(my_test_suite, "Test if hash table has same value in every entry!", test_has_value_all) == NULL) ||
	(CU_add_test(my_test_suite, "Remove middle entry", test_remove_multiple) == NULL) ||
	(CU_add_test(my_test_suite, "Change all values with apply function", test_apply_all) == NULL)
	  ){
      // If adding any of the tests fails, we tear down CUnit and exitp
      CU_cleanup_registry();
      return CU_get_error();
    }

  // Set the running mode. Use CU_BRM_VERBOSE for maximum output.
  // Use CU_BRM_NORMAL to only print errors and a summary
  CU_basic_set_mode(CU_BRM_VERBOSE);

  // This is where the tests are actually run!
  CU_basic_run_tests();

  // Tear down CUnit before exiting
  CU_cleanup_registry();
  return CU_get_error();
} 

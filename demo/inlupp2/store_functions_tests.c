#include <stdlib.h>
#include <string.h>

#include "stor_functions.h"
#include "hash_table-dynamic.h"
#include "linked_list.h"
#include "../../src/gc.h"

#include <stdbool.h>

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

heap_t *heap = NULL;

int init_suite(void) {
	// Change this function if you want to do something *before* you
	// run a test suite
	size_t bytes = 2048 * 4;
	bool unsafe_stack = false;
	float gc_threshold = 0.7;
	heap_t *heap = h_init(bytes, unsafe_stack, gc_threshold);
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

void test_create_database() {
	database_t *db = create_database(heap);

	CU_ASSERT_PTR_NOT_NULL(db);

}

void test_add_merchandise() {
    database_t *db = create_database(heap);
    ioopm_hash_table_t *all_merch = db->all_merch;

	char *name = h_strdup("test", heap);
    char *description =  h_strdup("testdes", heap);

    add_merchandise(name, description, 100, all_merch, heap);
    CU_ASSERT_PTR_NOT_NULL(ioopm_hash_table_lookup(all_merch, (ioopm_elem_t)"test").string);

}

void test_remove_merchandise() {
	database_t *db = create_database(heap);
	ioopm_hash_table_t *all_merch = db->all_merch;
	ioopm_hash_table_t *all_carts = db->all_carts;

	char *name = h_strdup("test", heap);
    char *description =  h_strdup("testdes", heap);

    add_merchandise(name, description, 100, all_merch, heap);

	CU_ASSERT_PTR_NOT_NULL(ioopm_hash_table_lookup(all_merch, (ioopm_elem_t)"test").string);
		
    remove_merchandise(all_carts,db->all_merch,name,heap);
    CU_ASSERT_PTR_NULL(ioopm_hash_table_lookup(all_merch, (ioopm_elem_t)"test").string);

}

static bool is_merch_in_shelfs(ioopm_list_t *shelfs, char *name, int amount) {

	ioopm_list_iterator_t *it_shelfs = ioopm_list_iterator(shelfs, heap);

	for(int i = 0; i < shelfs->size; i++) {
		ioopm_elem_t current = ioopm_iterator_current(it_shelfs);
		shelf_pair_t *shelf_pair = current.any;

		char *shelf_name = shelf_pair->shelf_name;

		printf("SHELF NAME %s\n",shelf_name);
		if(strcmp(name,shelf_name) == 0){
			return true;
		}
		ioopm_iterator_next(it_shelfs);
	}

	return false;
}

void test_replenish() {
	database_t *db = create_database(heap);
	ioopm_hash_table_t *all_merch = db->all_merch;

	char *name = h_strdup("test", heap);
	char *shelf = h_strdup("a22", heap);
    char *description =  h_strdup("hejsan", heap);

	add_merchandise(name, description, 100, all_merch, heap);

	CU_ASSERT_PTR_NOT_NULL(ioopm_hash_table_lookup(all_merch, (ioopm_elem_t)"test").string);
		
	replenish(all_merch,name,shelf, 10, heap);	

	merch_t *merch = ioopm_hash_table_lookup(all_merch, (ioopm_elem_t)"test").any;

	ioopm_list_t *shelfs = merch->shelfs;

	CU_ASSERT_TRUE(is_merch_in_shelfs(shelfs,shelf,10));

}


void test_create_cart() {
	database_t *db = create_database(heap);
	ioopm_hash_table_t *all_carts = db->all_carts;

	create_cart(all_carts,1, heap);

	CU_ASSERT_PTR_NOT_NULL(ioopm_hash_table_lookup(all_carts, (ioopm_elem_t)1).string);

}

void test_remove_cart() {
	database_t *db = create_database(heap);
	//ioopm_hash_table_t *all_merch = db->all_merch;
	ioopm_hash_table_t *all_carts = db->all_carts;

	create_cart(all_carts,1, heap);

	CU_ASSERT_PTR_NOT_NULL(ioopm_hash_table_lookup(all_carts, (ioopm_elem_t)1).string);
		
	remove_cart(1,all_carts, heap);

	CU_ASSERT_PTR_NULL(ioopm_hash_table_lookup(all_carts, (ioopm_elem_t)1).string);

}


static bool is_merch_in_cart(ioopm_hash_table_t *all_carts, char *name, int cart_key) {

	ioopm_elem_t lookup_cart = ioopm_hash_table_lookup(all_carts,(ioopm_elem_t)cart_key);
	ioopm_hash_table_t *cart = lookup_cart.any;
	ioopm_list_t *cart_pairs = ioopm_hash_table_values(cart, heap);
	ioopm_list_iterator_t *it_cart_pairs = ioopm_list_iterator(cart_pairs, heap);
   
	for(int i = 0; i < cart_pairs->size; i++) {
		ioopm_elem_t current = ioopm_iterator_current(it_cart_pairs);
		cart_pair_t *cart_pair = current.any;

		if(strcmp(cart_pair->merch->name,name) == 0) {
			return true;
		}
		ioopm_iterator_next(it_cart_pairs);      
	}

	return false;
}

static int num_for_shelfs(ioopm_list_t *shelfs) {
	ioopm_list_iterator_t *it_shelfs = ioopm_list_iterator(shelfs, heap);
	int num = 0;
	for(int i = 0; i < shelfs->size; i++) {
		ioopm_elem_t current = ioopm_iterator_current(it_shelfs);
		shelf_pair_t *shelf_pair = current.any;
		num += shelf_pair->num_of_merch;
		ioopm_iterator_next(it_shelfs);
	}
	return num; 
}

void test_add_to_cart() {
	database_t *db = create_database(heap);
	ioopm_hash_table_t *all_merch = db->all_merch;
	ioopm_hash_table_t *all_carts = db->all_carts;

	create_cart(all_carts,1, heap);

	char *name = h_strdup("test", heap);
    char *description =  h_strdup("testdes", heap);
    
	add_merchandise(name, description, 100, all_merch, heap);

	CU_ASSERT_PTR_NOT_NULL(ioopm_hash_table_lookup(all_merch, (ioopm_elem_t)"test").string);

	char *shelf = h_strdup("a22", heap);

	replenish(all_merch,name,shelf,10, heap);

	add_to_cart(all_merch,all_carts,1,name,3, heap);

	//check if the merchandise is inside the cart
	CU_ASSERT_TRUE(is_merch_in_cart(all_carts,name,1));
}

void test_remove_from_cart() {
    database_t *db = create_database(heap);
	ioopm_hash_table_t *all_merch = db->all_merch;
	ioopm_hash_table_t *all_carts = db->all_carts;

	create_cart(all_carts,1, heap);

	char *name = h_strdup("test", heap);
    char *description =  h_strdup("testdes", heap);
    
	add_merchandise(name, description, 100, all_merch, heap);

	CU_ASSERT_PTR_NOT_NULL(ioopm_hash_table_lookup(all_merch, (ioopm_elem_t)"test").string);

	char *shelf = h_strdup("a22", heap);

	replenish(all_merch,name,shelf,10, heap);

	add_to_cart(all_merch,all_carts,1,name,3, heap);
	remove_from_cart(all_carts,1,name,3, heap);
	//check if the merchandise is inside the cart
	CU_ASSERT_FALSE(is_merch_in_cart(all_carts,name,1));

}

void test_calculate() {
	database_t *db = create_database(heap);
	ioopm_hash_table_t *all_merch = db->all_merch;
	ioopm_hash_table_t *all_carts = db->all_carts;

	create_cart(all_carts,1, heap);

	char *name = h_strdup("test", heap);
    char *description =  h_strdup("testdes", heap);
    
	add_merchandise(name, description, 100, all_merch, heap);

	CU_ASSERT_PTR_NOT_NULL(ioopm_hash_table_lookup(all_merch, (ioopm_elem_t)"test").string);

	char *shelf = h_strdup("a22", heap);

	replenish(all_merch,name,shelf,10, heap);
  
	add_to_cart(all_merch,all_carts,1,name,3, heap);

	//cost should be 300
	CU_ASSERT_EQUAL(calculate_cost(all_carts,1, heap),300);

}

void test_checkout() {
	database_t *db = create_database(heap);
	ioopm_hash_table_t *all_merch = db->all_merch;
	ioopm_hash_table_t *all_carts = db->all_carts;

	create_cart(all_carts,1, heap);

	CU_ASSERT_PTR_NOT_NULL(ioopm_hash_table_lookup(all_carts, (ioopm_elem_t)1).string);

	char *name = h_strdup("test", heap);
    char *description =  h_strdup("testdes", heap);
    
	add_merchandise(name, description, 100, all_merch, heap);

	CU_ASSERT_PTR_NOT_NULL(ioopm_hash_table_lookup(all_merch, (ioopm_elem_t)"test").string);

	char *shelf = h_strdup("a22", heap);

	replenish(all_merch,name,shelf,10, heap);
  
	add_to_cart(all_merch,all_carts,1,name,3, heap);
	CU_ASSERT_TRUE(is_merch_in_cart(all_carts,name,1));

	checkout(all_carts,1, heap);
  
	CU_ASSERT_PTR_NULL(ioopm_hash_table_lookup(all_carts, (ioopm_elem_t)1).string);
	merch_t *merch = ioopm_hash_table_lookup(all_merch,(ioopm_elem_t)name).any;
	CU_ASSERT_EQUAL(num_for_shelfs(merch->shelfs),7);

}

//does the merch exist in cart if removed?
void test_remove_merch_check_cart() {
	database_t *db = create_database(heap);
	ioopm_hash_table_t *all_merch = db->all_merch;
	ioopm_hash_table_t *all_carts = db->all_carts;

	create_cart(all_carts,1, heap);

	CU_ASSERT_PTR_NOT_NULL(ioopm_hash_table_lookup(all_carts, (ioopm_elem_t)1).string);

	char *name = h_strdup("test", heap);
    char *description =  h_strdup("testdes", heap);
    
	add_merchandise(name, description, 100, all_merch, heap);

	CU_ASSERT_PTR_NOT_NULL(ioopm_hash_table_lookup(all_merch, (ioopm_elem_t)"test").string);
		
	char *shelf = h_strdup("a22", heap);

	replenish(all_merch,name,shelf,10, heap);
  
	add_to_cart(all_merch,all_carts,1,name,3, heap);
	CU_ASSERT_TRUE(is_merch_in_cart(all_carts,name,1));

	remove_merchandise(all_carts,all_merch,name, heap);

	
	CU_ASSERT_PTR_NULL(ioopm_hash_table_lookup(all_merch, (ioopm_elem_t)"test").string);

	CU_ASSERT_FALSE(is_merch_in_cart(all_carts,name,1));

}

//can carts hold more merchandise than it exists?
void test_cart_hold_maximum_merch() {
	database_t *db = create_database(heap);
	ioopm_hash_table_t *all_merch = db->all_merch;
	ioopm_hash_table_t *all_carts = db->all_carts;

	create_cart(all_carts,1, heap);
	create_cart(all_carts,2, heap);
	create_cart(all_carts,3, heap);

	char *name = h_strdup("test", heap);
    char *description =  h_strdup("testdes", heap);
    
	add_merchandise(name, description, 100, all_merch, heap);

	CU_ASSERT_PTR_NOT_NULL(ioopm_hash_table_lookup(all_merch, (ioopm_elem_t)"test").string);

	char *shelf = h_strdup("a22", heap);

	replenish(all_merch,name,shelf,10, heap);
  
	add_to_cart(all_merch,all_carts,1,name,3, heap);
	add_to_cart(all_merch,all_carts,2,name,4, heap);
	//should not be added
	add_to_cart(all_merch,all_carts,3,name,4, heap);


	CU_ASSERT_TRUE(is_merch_in_cart(all_carts,name,1));
	CU_ASSERT_TRUE(is_merch_in_cart(all_carts,name,2));
	CU_ASSERT_FALSE(is_merch_in_cart(all_carts,name,3));

	checkout(all_carts,1, heap);
	checkout(all_carts,2, heap);
	checkout(all_carts,3, heap);
  
	merch_t *merch = ioopm_hash_table_lookup(all_merch,(ioopm_elem_t)name).any;
	CU_ASSERT_EQUAL(num_for_shelfs(merch->shelfs),3);

}

void test_edit_merch() {
	database_t *db = create_database(heap);
	ioopm_hash_table_t *all_merch = db->all_merch;
	ioopm_hash_table_t *all_carts = db->all_carts;

	create_cart(all_carts,1, heap);
	
	CU_ASSERT_PTR_NOT_NULL(ioopm_hash_table_lookup(all_carts, (ioopm_elem_t)1).string);

	char *name = h_strdup("test", heap);
    char *description =  h_strdup("testdes", heap);
    
	add_merchandise(name, description, 100, all_merch, heap);

	CU_ASSERT_PTR_NOT_NULL(ioopm_hash_table_lookup(all_merch, (ioopm_elem_t)"test").string);

	char *shelf = h_strdup("a11", heap);
	
	replenish(all_merch,name,shelf, 10, heap);
		merch_t *merch = ioopm_hash_table_lookup(all_merch, (ioopm_elem_t)"test").any;
	ioopm_list_t *shelfs = merch->shelfs;
	
	CU_ASSERT_TRUE(is_merch_in_shelfs(shelfs,shelf,10));

	edit_merchandise(all_merch,all_carts,strdup(name),strdup("b"),strdup("a"),3, heap);

	//name = "b"

		merch_t *edit_merch = ioopm_hash_table_lookup(all_merch, (ioopm_elem_t)"b").any;
		
	ioopm_list_t *edit_shelfs = edit_merch->shelfs;
	
	CU_ASSERT_TRUE(is_merch_in_shelfs(edit_shelfs,"a11",10));

}




int main() {
    if (CUE_SUCCESS != CU_initialize_registry()) {
        return CU_get_error();
    }

    CU_pSuite suite = CU_add_suite("Merchandise_Test_Suite", init_suite, clean_suite);
    if (suite == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if ((CU_add_test(suite, "Create a database", test_create_database) == NULL) || (CU_add_test(suite, "Add one merchandise", test_add_merchandise) == NULL) || (CU_add_test(suite, "Replenish a merchandise", test_replenish) == NULL) || (CU_add_test(suite, "Remove one merchanise", test_remove_merchandise) == NULL) || (CU_add_test(suite, "Create one cart", test_create_cart) == NULL) || (CU_add_test(suite, "Remove one cart", test_remove_cart) == NULL) || (CU_add_test(suite, "add one merch to a cart", test_add_to_cart) == NULL) || (CU_add_test(suite, "Remove one merch from a cart", test_remove_from_cart) == NULL) || (CU_add_test(suite, "Calculate the cost of a cart", test_calculate) == NULL) || (CU_add_test(suite, "Checkout a cart", test_checkout) == NULL) || (CU_add_test(suite, "Check if a merch still exists in a cart after it is removed",test_remove_merch_check_cart) == NULL) || (CU_add_test(suite, "Carts can not hold more of a merchandise than it exists",test_cart_hold_maximum_merch) == NULL) || (CU_add_test(suite, "Edit a merchandise",test_edit_merch) == NULL)) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    
    CU_cleanup_registry();
    return CU_get_error();
}


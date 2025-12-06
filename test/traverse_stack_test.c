#include <CUnit/Basic.h>
#include "../src/traverse_stack.h"
#include <stdlib.h>
#include <string.h>
#include "../src/header/header.h"
#include "../src/header/bit_manip.h"
#include "../src/gc.h"

#define HEAP_SIZE (2048 * 4)
static heap_t *test_heap;

int init_suite(void)
{
    return 0;
}

int clean_suite(void)
{
    return 0;
}

void test_root_ptr_valid()
{
    /****** Valid Try *******/
    char *str[] = {"4i", "2d", "6c", "6*"};
    // "4i" => size(4*4) | 00010000 | layout 00000000 00000000 00000000 00000000 00000000 00000000 00000011
    // "2d" => size(4*4) | 00010000 | layout 00000000 00000000 00000000 00000000 00000000 00000000 00000011
    // "6c" => size(6*4) | 00011000 | layout 00000000 00000000 00000000 00000000 00000000 00000000 00000011
    // "6*" => size(12*4)| 00110000 | layout 11111100 00000000 00000000 00000000 00000000 00000000 00000011

    //heap
    size_t bytes = 4 * 2048;
    bool unsafe_stack = false;
    float tresh_hold = 0.8;
    heap_t *heap = h_init(bytes, unsafe_stack, tresh_hold);

    size_t count = 0;
    void **pointers;

    // 24bytes
    void* obj1 = h_alloc_struct(heap, str[0]);
    count = 0;
    pointers = root_ptrs(heap, &count);
    CU_ASSERT_TRUE(count > 0);


    //För att bevisa att vi har fått tag på addressen obj1 beffinner sig på
    char *hello = (char *)malloc(14 * sizeof(char));
    strcpy(hello, "Hello, World!");

    for(int i = 0; i < count; i++){
        *(void **)pointers[i] = hello;
    }
    
    CU_ASSERT_TRUE(strcmp(obj1, hello) == 0);

    
    // Clean up
    free(pointers);
}


void test_root_ptr_invalid()
{
    heap_t *test_heap = h_init(HEAP_SIZE, false, 0.75);
    size_t count = 0;
    void **pointers = root_ptrs(test_heap, &count);

    CU_ASSERT_EQUAL(count, 0);
    CU_ASSERT_PTR_NOT_NULL(pointers);
    free(pointers);
    count = 0;
    
    char* garbage = calloc(100, sizeof(char));
    void **ptrs_2 = root_ptrs(test_heap, &count);
    CU_ASSERT_EQUAL(count, 0);
    CU_ASSERT_PTR_NOT_NULL(ptrs_2);
    free(ptrs_2);
     
    uint64_t not_ptr = (uint64_t)test_heap->alloc_front;
    count = 0;
    void **ptrs_3 = root_ptrs(test_heap, &count);
    CU_ASSERT_EQUAL(count, 0);
    CU_ASSERT_PTR_NOT_NULL(ptrs_3);
    free(ptrs_3);

    h_delete(test_heap);
}

void test_invalidate_ptr(){
     /****** Valid Try *******/
    char *str[] = {"4i", "2d", "6c", "6*"};
    // "4i" => size(4*4) | 00010000 | layout 00000000 00000000 00000000 00000000 00000000 00000000 00000011
    // "2d" => size(4*4) | 00010000 | layout 00000000 00000000 00000000 00000000 00000000 00000000 00000011
    // "6c" => size(6*4) | 00011000 | layout 00000000 00000000 00000000 00000000 00000000 00000000 00000011
    // "6*" => size(12*4)| 00110000 | layout 11111100 00000000 00000000 00000000 00000000 00000000 00000011

    //heap
    size_t bytes = 4 * 2048;
    bool unsafe_stack = false;
    float tresh_hold = 0.8;
    heap_t *heap = h_init(bytes, unsafe_stack, tresh_hold);

    size_t count = 0;
    void **pointers;

    // 24bytes
    void* obj1 = h_alloc_struct(heap, str[0]);
    void* obj2 = h_alloc_struct(heap, str[0]);
    void* obj3 = h_alloc_struct(heap, str[0]);
    void* obj4 = h_alloc_struct(heap, str[0]);
    pointers = root_ptrs(heap, &count);
    
    void *dummy_value = (void *)0xDEADBEEF;
    invalidate_ptrs_at_heap(heap, dummy_value);

    CU_ASSERT_EQUAL(obj1, dummy_value);
    CU_ASSERT_EQUAL(obj2, dummy_value);
    CU_ASSERT_EQUAL(obj2, dummy_value);
    CU_ASSERT_EQUAL(obj3, dummy_value);
    free(pointers);
}




void test_ptr_valid()
{
    //TODO: fix more tests here
    char *str[] = {"4i", "2d", "6c", "6*"};
    // "4i" => size(4*4) | 00010000 | layout 00000000 00000000 00000000 00000000 00000000 00000000 00000011
    // "2d" => size(4*4) | 00010000 | layout 00000000 00000000 00000000 00000000 00000000 00000000 00000011
    // "6c" => size(6*4) | 00011000 | layout 00000000 00000000 00000000 00000000 00000000 00000000 00000011
    // "6*" => size(12*4)| 00110000 | layout 11111100 00000000 00000000 00000000 00000000 00000000 00000011

    // init 4 pages:
    size_t bytes = 4 * 2048;
    bool unsafe_stack = false;
    float tresh_hold = 0.8;
    heap_t *heap = h_init(bytes, unsafe_stack, tresh_hold);
    
    //GARBAGE 
    char* garbage = calloc(100, sizeof(char));

    // 32bytes
    void* obj1 = h_alloc_struct(heap, str[0]);
    uint64_t first_bytes = *heap->alloc_map;
    CU_ASSERT_TRUE(get_bit_at_index_from_msb(first_bytes, 0) >= 1);
    CU_ASSERT_TRUE(get_bit_at_index_from_msb(first_bytes, 1) >= 1);
    CU_ASSERT_FALSE(get_bit_at_index_from_msb(first_bytes, 2) >= 1);
    CU_ASSERT_TRUE(validate_ptr(heap, obj1));
    CU_ASSERT_FALSE(validate_ptr(heap, garbage));

    // 64, exact (4*16bytes), PROVES that our GC is compacting,
    /*
        Två likadana allokeringar, genererar olika antal av flips i alloc_map
    */
    void* obj2 = h_alloc_struct(heap, str[0]);
    first_bytes = *heap->alloc_map;
    CU_ASSERT_TRUE(get_bit_at_index_from_msb(first_bytes, 0) >= 1);
    CU_ASSERT_TRUE(get_bit_at_index_from_msb(first_bytes, 1) >= 1);
    CU_ASSERT_TRUE(get_bit_at_index_from_msb(first_bytes, 2) >= 1);
    CU_ASSERT_TRUE(get_bit_at_index_from_msb(first_bytes, 3) >= 1);
    CU_ASSERT_FALSE(get_bit_at_index_from_msb(first_bytes, 4) >= 1);
    CU_ASSERT_TRUE(validate_ptr(heap, obj2));
    CU_ASSERT_FALSE(validate_ptr(heap, garbage));
}

void test_find_header_by_moving_forward(){
  //TODO: fix more tests here
    char *str[] = {"4i", "2d", "6c", "6*"};
    // "4i" => size(4*4) | 00010000 | layout 00000000 00000000 00000000 00000000 00000000 00000000 00000011
    // "2d" => size(4*4) | 00010000 | layout 00000000 00000000 00000000 00000000 00000000 00000000 00000011
    // "6c" => size(6*4) | 00011000 | layout 00000000 00000000 00000000 00000000 00000000 00000000 00000011
    // "6*" => size(12*4)| 00110000 | layout 11111100 00000000 00000000 00000000 00000000 00000000 00000011

    // init 4 pages:
    size_t bytes = 4 * 2048;
    bool unsafe_stack = false;
    float tresh_hold = 0.8;
    heap_t *heap = h_init(bytes, unsafe_stack, tresh_hold);

    //lets allocate something
    void* obj1 = h_alloc_struct(heap, str[0]);

    //lets get a pointer to its header
    void* obj1_header = (char*)obj1 - 8;

    //now we will look in the first space
    void* end_addrs = heap->spaces[1]; //if we have to move pass space 2 we did something wrong

    void* found_header_address = find_header_address_through_footer(heap, obj1, end_addrs);

    CU_ASSERT_PTR_EQUAL(found_header_address, obj1_header);
    printf("DEBUG: obj1_header = %p\n", obj1_header);
    printf("DEBUG: found_header_address = %p\n", found_header_address);


}


int main()
{
    // Initialize CUnit test registry
    if (CU_initialize_registry() != CUE_SUCCESS)
        return CU_get_error();

    // Add a suite for the tests
    CU_pSuite my_test_suite = CU_add_suite("Stack traversal test suite", init_suite, clean_suite);
    if (my_test_suite == NULL)
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // Add individual test cases to the suite
    if ((NULL == CU_add_test(my_test_suite, "test_root_ptr_valid", test_root_ptr_valid)) ||
        (NULL == CU_add_test(my_test_suite, "test_root_ptr_invalid", test_root_ptr_invalid)) ||
        (NULL == CU_add_test(my_test_suite, "test_invalidate_ptr", test_invalidate_ptr)) ||
        (NULL == CU_add_test(my_test_suite, "test ptr valid", test_ptr_valid)) ||
        (NULL == CU_add_test(my_test_suite, "test find header by going forward", test_find_header_by_moving_forward))
        )
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // Set verbosity and run tests
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    // Cleanup CUnit registry
    CU_cleanup_registry();
    return CU_get_error();
}

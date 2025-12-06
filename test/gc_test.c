#include <CUnit/Basic.h>
#include <stdbool.h>
#include "../src/gc.h"
#include "../src/header/format_str.h"
#include "../src/header/bit_manip.h"
#include "../src/utils.h"

int init_suite(void)
{
    return 0;
}

int clean_suite(void)
{
    return 0;
}

//    heap_t *h_init(size_t bytes, bool unsafe_stack, float gc_threshold)

/**
 * Tests creating a heap with valid parameters!
 */
void test_heap_initialization_with_valid_parameters()
{
    size_t bytes_arr[] = {2048 * 4, 2048 * 6, 4 * 2048, 1000 * 2048};
    size_t lenght = 4;
    bool unsafe_stack = true;
    float tresh_hold = 0.8;

    for (int i = 0; i < lenght; i++)
    {
        size_t bytes = bytes_arr[i];
        heap_t *heap = h_init(bytes, unsafe_stack, tresh_hold);
        CU_ASSERT_PTR_NOT_NULL(heap);
        CU_ASSERT_EQUAL(heap->size, bytes);
        CU_ASSERT_EQUAL(heap->number_of_spaces, (size_t)((int)bytes / 2048));
        CU_ASSERT_PTR_NOT_NULL(heap->alloc_map);
        CU_ASSERT_EQUAL(*heap->alloc_map, 0x0);
        h_delete(heap);
    }
}

/*
 *  Tests creating a heap with invalid parameters
 * */
void test_heap_initialization_with_invalid_parameters()
{
    heap_t *heap_1 = h_init(0, true, 1.00);
    CU_ASSERT_PTR_NULL(heap_1);

    heap_t *heap_2 = h_init(2048, true, 1.00);
    CU_ASSERT_PTR_NULL(heap_2);

    heap_t *heap_3 = h_init(20000, true, 1.00);
    CU_ASSERT_PTR_NULL(heap_3);

    heap_t *heap_4 = h_init(2048 * 4, true, 1.00001);
    CU_ASSERT_PTR_NULL(heap_4);

    heap_t *heap_5 = h_init(2048 * 4, true, 0);
    CU_ASSERT_PTR_NULL(heap_5);

    heap_t *heap_6 = h_init(2048 * 4, true, -1.000);
    CU_ASSERT_PTR_NULL(heap_6);

    heap_t *heap_7 = h_init(2048 * 4, true, -0);
    CU_ASSERT_PTR_NULL(heap_7);

    heap_t *heap_8 = h_init(2048 * 3, true, 1.00);
    CU_ASSERT_PTR_NULL(heap_8);
}

void test_delete_heap()
{
    size_t bytes_arr[] = {2048 * 4, 2048 * 6, 10 * 2048, 1000 * 2048};
    size_t lenght = 4;
    bool unsafe_stack = true;
    float tresh_hold = 0.8;

    for (int i = 0; i < lenght; i++)
    {
        size_t bytes = bytes_arr[i];
        heap_t *heap = h_init(bytes, unsafe_stack, tresh_hold);
        h_delete(heap);
        CU_ASSERT_PTR_NULL(heap->space_is_active);
        CU_ASSERT_PTR_NULL(heap->spaces);
        CU_ASSERT_PTR_NULL(heap->start);
        CU_ASSERT_PTR_NULL(heap->alloc_front);
        CU_ASSERT_PTR_NULL(heap->alloc_map);
    }
}

// räcker det med test för invalidate_ptrs_at_heap
// TODO: Test the stuff that seperates it from normal delete
void test_delete_heap_dbg()
{
    void *dbg_value = (void *)0xDEADBEEF;

    size_t bytes_arr[] = {2048 * 4, 2048 * 6, 10 * 2048, 1000 * 2048};
    size_t lenght = 4;
    bool unsafe_stack = true;
    float tresh_hold = 0.8;

    for (int i = 0; i < lenght; i++)
    {
        size_t bytes = bytes_arr[i];
        heap_t *heap = h_init(bytes, unsafe_stack, tresh_hold);
        h_delete_dbg(heap, dbg_value);
        CU_ASSERT_PTR_NULL(heap->space_is_active);
        CU_ASSERT_PTR_NULL(heap->spaces);
        CU_ASSERT_PTR_NULL(heap->start);
        CU_ASSERT_PTR_NULL(heap->alloc_front);
        CU_ASSERT_PTR_NULL(heap->alloc_map);
    }
}

// test alloc two structs
void test_alloc_simple_struct_success()
{

    char *str[] = {"4i", "2d", "6c", "6*"};
    // "4i" => size(4*4) | 00010000 | layout 00000000 00000000 00000000 00000000 00000000 00000000 00000011
    // "2d" => size(4*4) | 00010000 | layout 00000000 00000000 00000000 00000000 00000000 00000000 00000011
    // "6c" => size(6*4) | 00011000 | layout 00000000 00000000 00000000 00000000 00000000 00000000 00000011
    // "6*" => size(12*4)| 00110000 | layout 11111100 00000000 00000000 00000000 00000000 00000000 00000011

    uint64_t expected_headers[] = {
        0x1000000000000003,
        0x1000000000000003,
        0x1800000000000003,
        0x30FC000000000003};

    size_t test_lenght = 4;
    void *pntr_to_object = NULL;

    // init 4 pages:
    size_t bytes = 4 * 2048;
    bool unsafe_stack = false;
    float tresh_hold = 0.8;
    heap_t *heap = h_init(bytes, unsafe_stack, tresh_hold);

    // alloc struct and make sure header is as expected and following mem empty
    for (int i = 0; i < test_lenght; i++)
    {
        pntr_to_object = h_alloc_struct(heap, str[i]);
        size_t size = get_size_bits_field(expected_headers[i]);
        // make sure all bytes are 0
        bool assert_flag = true;
        unsigned char *iter = pntr_to_object;
        for (int i = 0; i < size; i++)
        {
            if (*iter != 0x0)
            {
                assert_flag = false;
            }
            iter++;
        }
        CU_ASSERT_TRUE(assert_flag);
        // now point to header
        unsigned char *header_pntr = (unsigned char *)pntr_to_object;
        header_pntr -= 8;
        uint64_t header = *(uint64_t *)header_pntr;
        // check header is as expected
        CU_ASSERT_EQUAL(header, expected_headers[i]);
    }
}

void test_alloc_advanced_struct_success()
{
    char *str[] = {
        "2i*4ci4*i",
        "4c***dd*",
        "ddfll**dd*d*l*i*",
        "*ll**c3i***dd*d*"};

    uint64_t expected_headers[] = {
        0x4820F00000000003,
        0x400E100000000003,
        0x7800612500000003,
        0x78861C2400000003};

    size_t test_lenght = 4;
    void *pntr_to_object = NULL;

    // init 4 pages:
    size_t bytes = 4 * 2048;
    bool unsafe_stack = false;
    float tresh_hold = 0.8;
    heap_t *heap = h_init(bytes, unsafe_stack, tresh_hold);

    // alloc struct and make sure header is as expected and following mem empty
    for (int i = 0; i < test_lenght; i++)
    {
        pntr_to_object = h_alloc_struct(heap, str[i]);
        size_t size = get_size_bits_field(expected_headers[i]);
        // make sure all bytes are 0
        unsigned char *iter = pntr_to_object;
        bool is_false = true;
        for (int i = 0; i < size; i++)
        {
            if (*iter != 0x0)
            {
                is_false = false;
            }
            iter++;
        }
        CU_ASSERT_TRUE(is_false);
        // now point to header
        unsigned char *header_pntr = (unsigned char *)pntr_to_object;
        header_pntr -= 8;
        uint64_t header = *(uint64_t *)header_pntr;
        // check header is as expected
        CU_ASSERT_EQUAL(header, expected_headers[i]);
    }
}

// Test allocation with invalid format string
void test_alloc_struct_invalid_layout()
{
    heap_t *heap = h_init(2048 * 4, true, 0.7);

    size_t used = h_used(heap);
    size_t avail = h_avail(heap);

    char *format_str_arr[] = {
        //"b",
        "1000000000000*",
        "0c",
        //"c",
        NULL};

    int len = 3;

    for (int i = 0; i < len; i++)
    {
        void *ptr_to_struct = h_alloc_struct(heap, format_str_arr[i]);
        CU_ASSERT_EQUAL(used, h_used(heap));   // should not change since input invaild
        CU_ASSERT_EQUAL(avail, h_avail(heap)); // should not change since input invaild
        CU_ASSERT_PTR_NULL(ptr_to_struct);
    }

    h_delete(heap);
}

// test alloc two structs
void test_alloc_map_simple_struct_success()
{

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

<<<<<<< HEAD
    // 24bytes
=======
    // 8 + 16 + 8 => 32 bytes bytes
>>>>>>> footer
    h_alloc_struct(heap, str[0]);
    uint64_t first_bytes = *heap->alloc_map;
    CU_ASSERT_TRUE(get_bit_at_index_from_msb(first_bytes, 0) >= 1);
    CU_ASSERT_TRUE(get_bit_at_index_from_msb(first_bytes, 1) >= 1);
    CU_ASSERT_FALSE(get_bit_at_index_from_msb(first_bytes, 2) >= 1);
    // 8 + 16 + 8 => 32 + 32 bytes bytes
    h_alloc_struct(heap, str[0]);
    first_bytes = *heap->alloc_map;

    CU_ASSERT_TRUE(get_bit_at_index_from_msb(first_bytes, 0) >= 1);
    CU_ASSERT_TRUE(get_bit_at_index_from_msb(first_bytes, 1) >= 1);
    CU_ASSERT_TRUE(get_bit_at_index_from_msb(first_bytes, 2) >= 1);
    CU_ASSERT_TRUE(get_bit_at_index_from_msb(first_bytes, 3) >= 1);
    CU_ASSERT_FALSE(get_bit_at_index_from_msb(first_bytes, 4) >= 1);
}

void test_raw_invalid_cases()
{
    heap_t *heap = h_init(4 * 2048, true, 0.7);
    size_t bytes_arr[] = {0, 100000000}; // TODO: add more invaild inputs to raw alloc
    int len = 2;

    for (int i = 0; i < len; i++)
    {
        void *alloced = h_alloc_raw(heap, bytes_arr[i]);
        CU_ASSERT_PTR_NULL(alloced);
    }

    h_delete(heap);
}

void test_raw_valid_cases(){

    // init 4 pages:
    size_t bytes = 4 * 2048;
    bool unsafe_stack = false;
    float tresh_hold = 0.8;
    heap_t *heap = h_init(bytes, unsafe_stack, tresh_hold);

    //lets allocate rawww
    size_t first_alloc_bytes = 32; 
    void* raw_obj = h_alloc_raw(heap, first_alloc_bytes);

    //make sure that heap is 32 + 8 * 2 big
    uint64_t heap_expected_size = first_alloc_bytes + (HEADER_SIZE_BYTES*2);
    uint64_t heap_size = h_used(heap);
    CU_ASSERT_EQUAL(heap_expected_size, heap_size);

    uint64_t first_bytes = *heap->alloc_map;
    CU_ASSERT_TRUE(get_bit_at_index_from_msb(first_bytes, 0) >= 1);
    CU_ASSERT_TRUE(get_bit_at_index_from_msb(first_bytes, 1) >= 1);
    CU_ASSERT_TRUE(get_bit_at_index_from_msb(first_bytes, 2) >= 1);
    CU_ASSERT_FALSE(get_bit_at_index_from_msb(first_bytes,3) >= 1);
    
    print_bits(first_bytes);
}


void test_avil_invalid_cases()
{
    // TODO:lägg till fler
    char *str[] = {
        "x",
        "y",
        "0d",
        "100000000*",
        "100000000i"};

    size_t test_lenght = 5;
    void *pntr_to_object = NULL;

    // init 4 pages:
    size_t bytes = 4 * 2048;
    bool unsafe_stack = false;
    float tresh_hold = 0.8;
    heap_t *heap = h_init(bytes, unsafe_stack, tresh_hold);

    CU_ASSERT_EQUAL(heap->size, h_avail(heap));

    // alloc struct and make sure header is as expected and following mem empty
    for (int i = 0; i < test_lenght; i++)
    {
        pntr_to_object = h_alloc_struct(heap, str[i]);
        CU_ASSERT_EQUAL(heap->size, h_avail(heap));
    }
    h_delete(heap);
}

void test_avil_valid_cases()
{
    char *str[] = {
        "cccc",
        "****",
        "dddd",
        "ffff",
        "dddddddd*****ccciii",
        "iiiiii",
        "i*dc"
    };

    size_t test_lenght = 7;
    void *pntr_to_object = NULL;

    // init 4 pages:
    size_t bytes = 4 * 2048;
    bool unsafe_stack = false;
    float tresh_hold = 0.8;
    heap_t *heap = h_init(bytes, unsafe_stack, tresh_hold);

    size_t current_heap_size = 0;

    CU_ASSERT_EQUAL(heap->size, h_avail(heap));

    // alloc struct and make sure header is as expected and following mem empty
    for (int i = 0; i < test_lenght; i++)
    {
        pntr_to_object = h_alloc_struct(heap, str[i]);
        format_str_t *frmt_str = create_format_str(str[i]);
        current_heap_size += format_str_bytes(frmt_str) + HEADER_SIZE_BYTES*2;
        free(frmt_str);
        // probelm om första är fel vet man ej vilken som är problemet
        CU_ASSERT_EQUAL(heap->size - current_heap_size, h_avail(heap));
    }
    h_delete(heap);
}

// TODO: lägg till även från allloc raw
void test_used_invalid_cases()
{
    // TODO:lägg till fel
    char *str[] = {
        "x",
        "y",
        "",
        "****************************************************************",
        "dddddddd*****ccciii******dddddddddfffffffffiiiiiiiiiiiiiiii"};
    
    size_t test_lenght = 5;
    void *pntr_to_object = NULL;

    // init 4 pages:
    size_t bytes = 4 * 2048;
    bool unsafe_stack = false;
    float tresh_hold = 0.8;
    heap_t *heap = h_init(bytes, unsafe_stack, tresh_hold);

    CU_ASSERT_EQUAL(0, h_used(heap));

    // alloc struct and make sure header is as expected and following mem empty
    for (int i = 0; i < test_lenght; i++)
    {
        pntr_to_object = h_alloc_struct(heap, str[i]);
        CU_ASSERT_EQUAL(0, h_used(heap));
    }
    h_delete(heap);
}

// TODO: lägg till även från allloc raw
void test_used_valid_cases()
{
    char *str[] = {
        "cccc",
        "****",
        "dddd",
        "ffff",
        "dddddddd*****ccciii",
        "iiiiii",
        "i*dc"};

    size_t test_lenght = 7;
    void *pntr_to_object = NULL;

    // init 4 pages:
    size_t bytes = 4 * 2048;
    bool unsafe_stack = false;
    float tresh_hold = 0.8;
    heap_t *heap = h_init(bytes, unsafe_stack, tresh_hold);
    size_t current_heap_size = 0;

    CU_ASSERT_EQUAL(current_heap_size, h_used(heap));

    // alloc struct and make sure header is as expected and following mem empty
    for (int i = 0; i < test_lenght; i++)
    {
        pntr_to_object = h_alloc_struct(heap, str[i]);
        format_str_t *frmt_str = create_format_str(str[i]);
        current_heap_size += format_str_bytes(frmt_str) + HEADER_SIZE_BYTES*2;
        free(frmt_str);
        // probelm om första är fel vet man ej vilken som är problemet
        CU_ASSERT_EQUAL(current_heap_size, h_used(heap));
    }
    h_delete(heap);
}


<<<<<<< HEAD
=======


// ============   GC TESTS =====================


/*
 *  A test that shows that after the first link to an object is found, the header is now 
 *  a forward address and addresses[0] addresses[1] should point to new 
 */
void test_gc_struct_no_garabge_simple() {

    heap_t *heap = h_init(2048*6, false, 0.8);
    char *layout = "4i";
    void *original_obj = h_alloc_struct(heap, layout);
    
    void *original_header = (unsigned char*)original_obj - 8;

    original_header -= 0x00FFFFFFFF;

    //Test data
    uint64_t firstTwoIntegers = 0x00FF00FFFFFFFFFF;
    uint64_t secondTwoIntegers = 0x00FF00FF00FF00FF;

    uint64_t *ptr = (uint64_t *)original_obj;
    ptr[0] = firstTwoIntegers;  // First 8 bytes all 1's
    ptr[1] = secondTwoIntegers;  // Next 8 bytes all 1's   
    ptr += 1; 

    ptr = NULL;

    size_t after_garbage_collection = h_gc(heap);

    //go back to old header,and point back into the heap
    original_header += 0x00FFFFFFFF;

    //Proving that data was moved!
    uint64_t *new_ptr = (uint64_t *)original_obj;
    CU_ASSERT_EQUAL(new_ptr[0], secondTwoIntegers);

    //unmask so that we can use it as an address
    *(uint64_t*)original_header = set_bit_at_index_from_lsb(*(uint64_t*)original_header, 0, false);
    void* new_header = (void*)set_bit_at_index_from_lsb(*(uint64_t*)original_header, 0, false);

    void* new_header_through_ptr_arithmetic = ((unsigned char*)new_ptr - 8); 

    CU_ASSERT_PTR_EQUAL(new_header, new_header_through_ptr_arithmetic);

    CU_ASSERT_EQUAL(after_garbage_collection, 0); //as no item is collected
    h_delete(heap);
}


void test_gc_struct_no_garabge_multiple_references() {
    heap_t *heap = h_init(2048*6, false, 0.8);
    char *layout = "4i";
    void *original_obj = h_alloc_struct(heap, layout);
    
    void *original_header = (unsigned char*)original_obj - 8;
    original_header -= 0x00FFFFFFFF;

    //Test data
    uint64_t firstTwoIntegers = 0x00FF00FFFFFFFFFF;
    uint64_t secondTwoIntegers = 0x00FF00FFFFFFFFFF;

    uint64_t *ptr = (uint64_t *)original_obj;
    ptr[0] = firstTwoIntegers;  // First 8 bytes all 1's
    ptr[1] = secondTwoIntegers;  // Next 8 bytes all 1's   
    ptr = NULL; 


    //should also be moved but treated as forward address (only because obj is treated first)
    void *addresses[5];
    addresses[0] = original_obj;
    addresses[1] = original_obj;
    addresses[2] = original_obj;
    addresses[3] = original_obj;
    addresses[4] = original_obj;

    size_t after_garbage_collection = h_gc(heap);

    //go back to old header,and point back into the heap
    original_header += 0x00FFFFFFFF;

    //Proving that data was moved!
    uint64_t *new_ptr = (uint64_t *)original_obj;
    CU_ASSERT_EQUAL(new_ptr[0], firstTwoIntegers);
    CU_ASSERT_EQUAL(new_ptr[1], secondTwoIntegers);
    
    //unmask so that we can use it as an address
    *(uint64_t*)original_header = set_bit_at_index_from_lsb(*(uint64_t*)original_header, 0, false);
    void* new_header = (void*)set_bit_at_index_from_lsb(*(uint64_t*)original_header, 0, false);
    void* new_header_through_ptr_arithmetic = ((unsigned char*)new_ptr - 8); 

    CU_ASSERT_PTR_EQUAL(new_header, new_header_through_ptr_arithmetic);

    //make sure addresses[0] & addresses[1] booth points at original_object
    CU_ASSERT_PTR_EQUAL(addresses[0], original_obj);
    CU_ASSERT_PTR_EQUAL(addresses[1], original_obj); 
    
    CU_ASSERT_EQUAL(after_garbage_collection, 0); //as no item is collected
    h_delete(heap);
}



>>>>>>> footer
void test_gc_raw_no_garbage(){
    /*
    heap_t *heap = h_init(2048*6, false, 0.8);
    void *obj = h_alloc_raw(heap, 8);
    size_t after_garbage_collection = h_gc(heap);
    CU_ASSERT_EQUAL(after_garbage_collection, 0);
    h_delete(heap);
    */
}


void test_gc_raw_one_garbage(){
    /*
    heap_t *heap = h_init(2048*6, false, 0.8);
    void *obj = h_alloc_raw(heap, 8);
    obj = NULL;
    size_t after_garbage_collection = h_gc(heap);
    CU_ASSERT_EQUAL(after_garbage_collection, 24);
    h_delete(heap);
    */
}

void test_gc_raw_more_garbage(){
    /*
    heap_t *heap = h_init(2048*6, false, 0.8);
    void *obj = h_alloc_raw(heap, 8);
    void *obj2 = h_alloc_raw(heap, 10);
    void *obj3 = h_alloc_raw(heap, 10);
    void *obj4 = h_alloc_raw(heap, 13);
    void *obj5 = h_alloc_raw(heap, 15); 
    obj = NULL; 
    obj2 = NULL;
    obj5 = NULL;
    size_t after_garbage_collection = h_gc(heap);
<<<<<<< HEAD
    CU_ASSERT_EQUAL(after_garbage_collection, 57); 
=======
    CU_ASSERT_EQUAL(after_garbage_collection, 81); 
    h_delete(heap);
    */
}


void test_gc_struct_no_garabge_mutliple_of_8() {
    
    heap_t *heap = h_init(2048*6, false, 0.8);

    char *layout = "7*";
    char *layout2 = "10*";
    char *layout3 = "5*";

    void *obj = h_alloc_struct(heap, layout);
    void *obj2 = h_alloc_struct(heap, layout2);
    void *obj3 = h_alloc_struct(heap, layout3);

    
    size_t after_garbage_collection = h_gc(heap);
    printf("%zu\n", after_garbage_collection);
    CU_ASSERT_EQUAL(after_garbage_collection, 0); //as no item is collected

>>>>>>> footer
    h_delete(heap);
}

// TODO: failar just nu: så fixa så alloc är multipel av 8
void test_gc_struct_no_garabge_non_mutliple_of_8() {
    
    heap_t *heap = h_init(2048*6, false, 0.8);
<<<<<<< HEAD
    char *layout = "7i";
    //char *layout2 = "10i";
    //char *layout3 = "5i";
    void *obj = h_alloc_struct(heap, layout);
    //void *obj2 = h_alloc_struct(heap, layout2);
    //void *obj3 = h_alloc_struct(heap, layout3);
=======

    char *layout = "7i";
    char *layout2 = "10i";
    char *layout3 = "5i";

    void *obj = h_alloc_struct(heap, layout);
    void *obj2 = h_alloc_struct(heap, layout2);
    void *obj3 = h_alloc_struct(heap, layout3);

    
>>>>>>> footer
    size_t after_garbage_collection = h_gc(heap);
    printf("%zu\n", after_garbage_collection);
    CU_ASSERT_EQUAL(after_garbage_collection, 0); //as no item is collected
<<<<<<< HEAD
    free(layout);
    //free(layout2);
    //free(layout3);
=======

>>>>>>> footer
    h_delete(heap);
}

void test_gc_struct_one_garabge() {
    
    heap_t *heap = h_init(2048*6, false, 0.8);
    char *layout = "2i";
    char *layout2 = "10i";
    void *obj = h_alloc_struct(heap, layout);
    void *obj2 = h_alloc_struct(heap, layout2);
    obj2 = NULL;
    size_t after_garbage_collection = h_gc(heap);
<<<<<<< HEAD
    CU_ASSERT_EQUAL(after_garbage_collection, 48); //as the one being collcted was obj2
    free(layout);
    free(layout2);
=======
    CU_ASSERT_EQUAL(after_garbage_collection, 56); //as the one being collcted was obj2
>>>>>>> footer
    h_delete(heap);
    
}

void test_gc_one_garabge_with_pointer() {
    
    heap_t *heap = h_init(2048*6, false, 0.8);
    char *layout = "3i*";
    void *obj = h_alloc_struct(heap, layout);
    obj = NULL;
    size_t after_garbage_collection = h_gc(heap);
    CU_ASSERT_EQUAL(after_garbage_collection, 36); //as nothing is collcted
    h_delete(heap);
    
}

void test_multiple_gc() {
    
    heap_t *heap = h_init(2048*6, false, 0.8);
    char *layout = "2i";
    char *layout2 = "10i";
    void *obj = h_alloc_struct(heap, layout);
    void *obj2 = h_alloc_struct(heap, layout2);
    obj = NULL;
    size_t after_garbage_collection_first = h_gc(heap);
    CU_ASSERT_EQUAL(after_garbage_collection_first, 24);
    size_t after_garbage_collection_second = h_gc(heap);
    CU_ASSERT_EQUAL(after_garbage_collection_second, 0);
    obj2 = NULL;
    size_t after_garbage_collection_last = h_gc(heap);
    CU_ASSERT_EQUAL(after_garbage_collection_last, 56);
    
}


<<<<<<< HEAD
void test_gc_one_garabge_with_pointer() {
    heap_t *heap = h_init(2048*6, false, 0.8);
    char *layout = "3i*";
    void *obj = h_alloc_struct(heap, layout);
    obj = NULL;
    size_t after_garbage_collection = h_gc(heap);
    CU_ASSERT_EQUAL(after_garbage_collection, 28); //as nothing is collcted
    free(layout);
    h_delete(heap);
    
}
=======



//DONT REMOVE ACUTAL TESTS FOR LATER!!!!!!!

// void test_raw_and_struct_alloc_no_garbage() {
//     heap_t *heap = h_init(2048*6, false, 0.8);
//     char *layout = "2i";
//     void *obj = h_alloc_struct(heap, layout);
//     void *obj2 = h_alloc_raw(heap, 12);
//     size_t after_garbage_collection = h_gc(heap);
//     CU_ASSERT_EQUAL(after_garbage_collection, 0); 
//     h_delete(heap);
// }

// void test_raw_and_struct_alloc_struct_garbage() {
//     heap_t *heap = h_init(2048*6, false, 0.8);
//     char *layout = "2i";
//     void *obj = h_alloc_struct(heap, layout);
//     void *obj2 = h_alloc_raw(heap, 12);
//     obj = NULL;
//     size_t after_garbage_collection = h_gc(heap);
//     CU_ASSERT_EQUAL(after_garbage_collection, 20); 
//     h_delete(heap);
// }
>>>>>>> footer

void test_multiple_gc() {
    heap_t *heap = h_init(2048*6, false, 0.8);
    char *layout = "2i";
    char *layout2 = "10i";
    void *obj = h_alloc_struct(heap, layout);
    void *obj2 = h_alloc_struct(heap, layout2);
    obj = NULL;
    size_t after_garbage_collection_first = h_gc(heap);
    CU_ASSERT_EQUAL(after_garbage_collection_first, 16);
    size_t after_garbage_collection_second = h_gc(heap);
    CU_ASSERT_EQUAL(after_garbage_collection_second, 0);
    obj2 = NULL;
    size_t after_garbage_collection_last = h_gc(heap);
    CU_ASSERT_EQUAL(after_garbage_collection_last, 48);
}

<<<<<<< HEAD
=======
// void test_raw_and_struct_alloc_raw_garbage() {
//     heap_t *heap = h_init(2048*6, false, 0.8);
//     char *layout = "2i";
//     void *obj = h_alloc_struct(heap, layout);
//     void *obj2 = h_alloc_raw(heap, 12);
//     obj2 = NULL;
//     size_t after_garbage_collection = h_gc(heap);
//     CU_ASSERT_EQUAL(after_garbage_collection, 28); 
//     h_delete(heap);
// }

// void test_raw_and_struct_alloc_both_garbage() {
//     heap_t *heap = h_init(2048*6, false, 0.8);
//     char *layout = "2i";
//     void *obj = h_alloc_struct(heap, layout);
//     void *obj2 = h_alloc_raw(heap, 12);
//     obj = NULL;
//     obj2 = NULL;
//     size_t after_garbage_collection = h_gc(heap);
//     CU_ASSERT_EQUAL(after_garbage_collection, 28 + 20); 
//     h_delete(heap);
// }

//For circularreferencse
>>>>>>> footer
typedef struct link Link_t;
typedef struct link {
    Link_t* next;
};
<<<<<<< HEAD
=======

void test_circular_refernces_no_garbage(){
    /*
    heap_t *heap = h_init(2048*6, false, 0.8);
    Link_t *link_1 = h_alloc_struct(heap, "*");
    link_1->next = h_alloc_struct(heap, "*");
    link_1->next->next = h_alloc_struct(heap, "*");
    link_1->next->next->next = link_1;
    // tas upp imorgon
    
    size_t after_gc_1 = h_gc(heap);
    CU_ASSERT_EQUAL(after_gc_1, 0);
    */
}

void test_circular_refernces_garbage(){
    /*
    heap_t *heap = h_init(2048*6, false, 0.8);
    Link_t *link_1 = h_alloc_struct(heap, "*");
    link_1->next = h_alloc_struct(heap, "*");
    link_1->next->next = h_alloc_struct(heap, "*");
    link_1->next->next->next = link_1;
    // tas upp imorgon

    link_1 = NULL;
    
    size_t after_gc_1 = h_gc(heap);
    CU_ASSERT_EQUAL(after_gc_1, 0);
    */
}


>>>>>>> footer

void test_circular_refernces(){
    heap_t *heap = h_init(2048*6, false, 0.8);
    Link_t *link_1 = h_alloc_struct(heap, "*");
    link_1->next = h_alloc_struct(heap, "*");
    link_1->next->next = h_alloc_struct(heap, "*");
    link_1->next->next->next = link_1;
    // tas upp imorgon
    
    size_t after_gc_1 = h_gc(heap);
    CU_ASSERT_EQUAL(after_gc_1, 0);
}

int main()
{
    // Init CUnit test registry
    if (CU_initialize_registry() != CUE_SUCCESS)
        return CU_get_error();

    // Test suit
    CU_pSuite test_suite = CU_add_suite("GC test suite", init_suite, clean_suite);
    if (test_suite == NULL)
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if (
<<<<<<< HEAD
        /*(NULL == CU_add_test(test_suite, "test_heap_initialization_with_valid_parameters", test_heap_initialization_with_valid_parameters)) ||
=======
        (NULL == CU_add_test(test_suite, "test_heap_initialization_with_valid_parameters", test_heap_initialization_with_valid_parameters)) ||
>>>>>>> footer
        (NULL == CU_add_test(test_suite, "test_heap_initialization_with_invalid_parameters", test_heap_initialization_with_invalid_parameters)) ||
        (NULL == CU_add_test(test_suite, "test_delete_heap", test_delete_heap)) ||
        (NULL == CU_add_test(test_suite, "test_alloc_simple_struct_success", test_alloc_simple_struct_success)) ||
        (NULL == CU_add_test(test_suite, "test_alloc_advanced_struct_success", test_alloc_advanced_struct_success)) ||
        (NULL == CU_add_test(test_suite, "test_alloc_struct_invalid_layout", test_alloc_struct_invalid_layout)) ||
        (NULL == CU_add_test(test_suite, "test_alloc_map_struct_invalid_layout", test_alloc_map_simple_struct_success)) ||
        (NULL == CU_add_test(test_suite, "test_raw_valid_cases", test_raw_valid_cases)) ||
        (NULL == CU_add_test(test_suite, "test_raw_invalid_cases", test_raw_invalid_cases)) ||
        (NULL == CU_add_test(test_suite, "test_avil_invalid_cases", test_avil_invalid_cases)) ||
        (NULL == CU_add_test(test_suite, "test_avil_valid_cases", test_avil_valid_cases)) ||
        (NULL == CU_add_test(test_suite, "test_used_invalid_cases", test_used_invalid_cases)) ||
<<<<<<< HEAD
        (NULL == CU_add_test(test_suite, "test_used_vaild_cases", test_used_valid_cases)) || */
        (NULL == CU_add_test(test_suite, "test_gc_struct_no_garabge", test_gc_struct_no_garabge)) /*|| 
        (NULL == CU_add_test(test_suite, "test_gc_struct_one_garabge", test_gc_struct_one_garabge))|| 
        (NULL == CU_add_test(test_suite, "test_gc_raw_no_garbage", test_gc_raw_no_garbage))|| 
        (NULL == CU_add_test(test_suite, "test_gc_raw_one_garbage", test_gc_raw_one_garbage))|| 
        (NULL == CU_add_test(test_suite, "test_gc_one_garabge_with_pointer", test_gc_one_garabge_with_pointer))|| 
        (NULL == CU_add_test(test_suite, "test_circular_refernces", test_circular_refernces))|| 
        (NULL == CU_add_test(test_suite, "test_multiple_gc", test_multiple_gc))|| 
        (NULL == CU_add_test(test_suite, "test_gc_raw_more_garbage", test_gc_raw_more_garbage))*/)
=======
        (NULL == CU_add_test(test_suite, "test_used_vaild_cases", test_used_valid_cases)) || 
        (NULL == CU_add_test(test_suite, "test_gc_struct_no_garabge_mutliple_of_8", test_gc_struct_no_garabge_mutliple_of_8)) ||
        (NULL == CU_add_test(test_suite, "test_gc_struct_no_garabge_non_mutliple_of_8", test_gc_struct_no_garabge_non_mutliple_of_8)) ||
        (NULL == CU_add_test(test_suite, "test_gc_struct_no_garabge_simple", test_gc_struct_no_garabge_multiple_references)) ||
        (NULL == CU_add_test(test_suite, "test_gc_struct_no_garabge_multiple_references", test_gc_struct_no_garabge_multiple_references)) ||
        (NULL == CU_add_test(test_suite, "test_gc_struct_one_garabge", test_gc_struct_one_garabge)) ||
        (NULL == CU_add_test(test_suite, "test_gc_raw_no_garbage", test_gc_raw_no_garbage))||
        (NULL == CU_add_test(test_suite, "test_gc_raw_one_garbage", test_gc_raw_one_garbage))||
        (NULL == CU_add_test(test_suite, "test_gc_one_garabge_with_pointer", test_gc_one_garabge_with_pointer))||
        (NULL == CU_add_test(test_suite, "test_circular_refernces", test_circular_refernces_garbage))||
        (NULL == CU_add_test(test_suite, "test_multiple_gc", test_multiple_gc))||
        (NULL == CU_add_test(test_suite, "test_gc_raw_more_garbage", test_gc_raw_more_garbage))
        )
>>>>>>> footer
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // Test verbosity
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    // Cleanup
    CU_cleanup_registry();
    return CU_get_error();
}
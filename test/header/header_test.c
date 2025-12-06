#include <CUnit/Basic.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "../../src/header/header.h"

int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }

//TODO: write test testing get_header_type
void test_set_header_forward_address() {
    uint64_t garbageheader_value = 0x4820F00000000003;
    uint64_t address = 0x55aa23f1b000;
    uint64_t forward_address = 0x55AA23F1B001;

    header_t* header = header_new(garbageheader_value);
    set_header_forward_address(header, (void *)address);
    CU_ASSERT_EQUAL(forward_address, header->data);
};

int main() {
    if (CU_initialize_registry() != CUE_SUCCESS)
        return CU_get_error();

    // test suite
    CU_pSuite header_test_suite = CU_add_suite("Header Test Suite", init_suite, clean_suite);
    if (!header_test_suite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // individual test cases
    if (
        (NULL == CU_add_test(header_test_suite, "test set header forward address", test_set_header_forward_address))
        )
         {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    CU_cleanup_registry();
    return CU_get_error();
}
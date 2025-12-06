//
// Created by Viktor Liljenberg on 2024-12-20.
//
#include <CUnit/Basic.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "../../src/header/bit_vector.h"
#include "../../src/header/format_str.h"

int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }

void empty_test() {
    // Future implementation
}

int main() {
    if (CU_initialize_registry() != CUE_SUCCESS)
        return CU_get_error();

    // Add test suite
    CU_pSuite forward_adress_test_suite = CU_add_suite("Forward_adress", init_suite, clean_suite);
    if (!forward_adress_test_suite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // Add individual test cases
    if (
        (NULL == CU_add_test(forward_adress_test_suite, "Test empty", empty_test))
    ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // Run tests
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    // Clean up
    CU_cleanup_registry();
    return CU_get_error();
}
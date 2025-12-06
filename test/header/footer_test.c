//
// Created by Viktor Liljenberg on 2024-01-08.
//
#include <CUnit/Basic.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../../src/header/footer.h"

// I think this is not needed, and it is triggering error for the github CI
// #include <CUnit.h>

#include "../../src/header/format_str.h"

int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }

//confirm tag
void test_footer()
{
    format_str_t *format = create_format_str("2i*4ci4*i");
    bit_vector_t bv = to_bit_vector(format).bit_vector;

    uint8_t size = get_size_bits_field(bv);
    uint64_t footer = create_footer(size);

    CU_ASSERT_TRUE(confirm_tag(footer));
    uint64_t footer_size = get_size_bits_field(footer);
    CU_ASSERT_EQUAL(size, footer_size);
}

int main()
{
    if (CU_initialize_registry() != CUE_SUCCESS)
        return CU_get_error();

    // test suite
    CU_pSuite footer_test_suite = CU_add_suite("Footer Test Suite", init_suite, clean_suite);
    if (!footer_test_suite)
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // individual test cases
    if (
        (NULL == CU_add_test(footer_test_suite, "test footer", test_footer)) )

    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    CU_cleanup_registry();
    return CU_get_error();
}
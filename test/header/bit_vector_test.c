//
// Created by Viktor Liljenberg on 2024-12-19.
//
#include <CUnit/Basic.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "../../src/header/bit_vector.h"

// I think this is not needed, and it is triggering error for the github CI
// #include <CUnit.h>

#include "../../src/header/format_str.h"

int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }

bool test_helper_enough_bits_for_layout(const char *str)
{
    format_str_t *format = create_format_str((char *)str);
    bool result = enough_bits_for_layout(format);
    destroy_format_str(format);
    return result;
}

bool test_bit_vector_helper(char *input, uint64_t expected)
{
    format_str_t *format = create_format_str((char *)input);
    option_t result = to_bit_vector(format);
    destroy_format_str(format);
    if (!result.success)
    {
        printf("Test failed with input: %s Function failed to generate bit vector\n", input);
        return false;
    }

    bool is_equal = result.bit_vector == expected;
    if (is_equal)
    {
        return true;
    }

    printf("Test failed for input: %s\n", input);
    printf("  Expected: 0x%016llx\n", expected);
    printf("  Got:      0x%016llx\n", result.bit_vector);
    return false;
}

// Test cases for valid format strings
void test_enough_bits_for_layout_valid_cases()
{
    CU_ASSERT_TRUE(test_helper_enough_bits_for_layout("c"));
    CU_ASSERT_TRUE(test_helper_enough_bits_for_layout("i"));
    CU_ASSERT_TRUE(test_helper_enough_bits_for_layout("d"));
    CU_ASSERT_TRUE(test_helper_enough_bits_for_layout("10c"));
    CU_ASSERT_TRUE(test_helper_enough_bits_for_layout("2i3c"));
    CU_ASSERT_TRUE(test_helper_enough_bits_for_layout("*"));
    CU_ASSERT_TRUE(test_helper_enough_bits_for_layout("2*3i"));
    CU_ASSERT_TRUE(test_helper_enough_bits_for_layout("10d2c"));
    CU_ASSERT_TRUE(test_helper_enough_bits_for_layout("5i5c2*"));
    CU_ASSERT_TRUE(test_helper_enough_bits_for_layout("1i1c1*1d"));
    CU_ASSERT_TRUE(test_helper_enough_bits_for_layout("20i"));
}

void test_enough_bits_for_layout_edge_cases()
{
    // mixed variation of valid and invalid cases
    CU_ASSERT_TRUE(test_helper_enough_bits_for_layout("1c10i"));
    CU_ASSERT_FALSE(test_helper_enough_bits_for_layout("1000000i"));
    //CU_ASSERT_FALSE(test_helper_enough_bits_for_layout("100c"));
    CU_ASSERT_FALSE(test_helper_enough_bits_for_layout("100"));

    // mixed valid input and varying cases
    CU_ASSERT_TRUE(test_helper_enough_bits_for_layout("2*10c5i"));
    //CU_ASSERT_FALSE(test_helper_enough_bits_for_layout("100d100d100d"));
}

void test_to_bitvector()
{
    struct test_touple
    {
        char *input;
        uint64_t expected;
    };

    // An array of test data
    struct test_touple tests[] = {
        // size 01001000 | layout | 00100000 11110000 00000000 00000000 00000000 00000000 000000 | type 11
        // binary:0100100000100000111100000000000000000000000000000000000000000011
        {"2i*4ci4*i", 0x4820F00000000003},
        // size 01000000 | layout | 00001110 00010000 00000000 00000000 00000000 00000000 000000 | type 11
        // binary:0100000000001110000100000000000000000000000000000000000000000011
        {"4c***dd*", 0x400E100000000003},
        // size 01111000 | layout | 00000000 01100001 00100101 00000000 00000000 00000000 000000 | type 11
        // binary:0111100000000000011000010010010100000000000000000000000000000011
        {"ddfll**dd*d*l*i*", 0x7800612500000003},
        // size 01111000 | layout | 10000110 00011100 00100100 00000000 00000000 00000000 000000 | type 11
        // binary:0111100010000110000111000010010000000000000000000000000000000011
        {"*ll**c3i***dd*d*", 0x78861C2400000003}};

    // iterate through tests and call test_bit_vector_helper(char *input, uint64_t expected)

    // get size of tests
    size_t num_tests = sizeof(tests) / sizeof(tests[0]);
    for (size_t i = 0; i < num_tests; ++i)
    {
        // call assert true on each test
        CU_ASSERT_TRUE(test_bit_vector_helper(tests[i].input, tests[i].expected));
    }
}

void test_get_size_field()
{
    // using test data from test_to_bitvector()

    format_str_t *format = create_format_str("2i*4ci4*i");
    bit_vector_t bv = to_bit_vector(format).bit_vector;
    CU_ASSERT_EQUAL(get_size_bits_field(bv), 72);

    format = create_format_str("4c***dd*");
    bv = to_bit_vector(format).bit_vector;
    CU_ASSERT_EQUAL(get_size_bits_field(bv), 64);

    format = create_format_str("ddfll**dd*d*l*i*");
    bv = to_bit_vector(format).bit_vector;
    CU_ASSERT_EQUAL(get_size_bits_field(bv), 120);

    format = create_format_str("*ll**c3i***dd*d*");
    bv = to_bit_vector(format).bit_vector;
    CU_ASSERT_EQUAL(get_size_bits_field(bv), 120);
}

void test_get_layout_field()
{

    uint64_t expected = (uint64_t)0x20F0000000000000;
    format_str_t *format = create_format_str("2i*4ci4*i");
    bit_vector_t bv = to_bit_vector(format).bit_vector;
    CU_ASSERT_EQUAL(get_layout_bits_field(bv), expected);

    expected = (uint64_t)0xE10000000000000;
    format = create_format_str("4c***dd*");
    bv = to_bit_vector(format).bit_vector;
    CU_ASSERT_EQUAL(get_layout_bits_field(bv), expected);

    expected = (uint64_t)0x61250000000000;
    format = create_format_str("ddfll**dd*d*l*i*");
    bv = to_bit_vector(format).bit_vector;
    CU_ASSERT_EQUAL(get_layout_bits_field(bv), expected);

    expected = (uint64_t)0x861C240000000000;
    format = create_format_str("*ll**c3i***dd*d*");
    bv = to_bit_vector(format).bit_vector;
    CU_ASSERT_EQUAL(get_layout_bits_field(bv), expected);
}

void test_bit_vector_to_frmt_string() {
    char *str[] = {"iiii", "iiii", "iiiiii", "******"};
    // "4i" => size(4*4) | 00010000 | layout 00000000 00000000 00000000 00000000 00000000 00000000 00000011
    // "2d" => size(4*4) | 00010000 | layout 00000000 00000000 00000000 00000000 00000000 00000000 00000011
    // "6c" => size(6*4) | 00011000 | layout 00000000 00000000 00000000 00000000 00000000 00000000 00000011
    // "6*" => size(12*4)| 00110000 | layout 11111100 00000000 00000000 00000000 00000000 00000000 00000011

    uint64_t headers[] = {
        0x1000000000000003,
        0x1000000000000003,
        0x1800000000000003,
        0x30FC000000000003
    };

    size_t num_tests = 4;

    for (int i = 0; i < num_tests; ++i) {
        format_str_t* str1 = bit_vector_to_format_string(headers[i]);
        CU_ASSERT_TRUE(strcmp(str[i], str1->str) == 0);
        destroy_format_str(str1);
    }
}

int main()
{
    if (CU_initialize_registry() != CUE_SUCCESS)
        return CU_get_error();

    // test suite
    CU_pSuite bit_vector_test_suite = CU_add_suite("Bit Vector Test Suite", init_suite, clean_suite);
    if (!bit_vector_test_suite)
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // individual test cases
    if (
        (NULL == CU_add_test(bit_vector_test_suite, "Test To Bit Vector", test_to_bitvector)) ||
        (NULL == CU_add_test(bit_vector_test_suite, "Test Valid Cases", test_enough_bits_for_layout_valid_cases)) ||
        (NULL == CU_add_test(bit_vector_test_suite, "Test Edge Cases", test_enough_bits_for_layout_edge_cases)) ||
        (NULL == CU_add_test(bit_vector_test_suite, "Test get size field", test_get_size_field)) ||
        (NULL == CU_add_test(bit_vector_test_suite, "Test get layout field", test_get_layout_field)) ||
        (NULL == CU_add_test(bit_vector_test_suite, "Test bitvec -> frmt_string", test_bit_vector_to_frmt_string)) )

    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    CU_cleanup_registry();
    return CU_get_error();
}

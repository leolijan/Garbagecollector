#include <CUnit/Basic.h>
#include <stdio.h>
#include <stdlib.h>
#include "../../src/header/format_str.h" // Replace with the actual header file where format_str is declared.

int init_suite(void)
{
    return 0;
}

int clean_suite(void)
{
    return 0;
}



bool is_interpreted_format_str_as_expected(char *input, char *expected)
{
    char *result = interpreted_format_str(input);
    if (result == NULL)
    {
        printf("Error: interpreted_format_str returned NULL for input '%s'\n", input);
        return false;
    }

    bool is_equal = strcmp(result, expected) == 0;
    if (!is_equal)
    {
        printf("Error: For input '%s', expected '%s' but got '%s'\n", input, expected, result);
    }

    free(result); // Free allocated memory
    return is_equal;
}

void test_interpret_stringformat()
{
    // Basic valid cases
    CU_ASSERT_TRUE(is_interpreted_format_str_as_expected("3i**", "iii**"));    // Repeated 'i' and unchanged '*'
    CU_ASSERT_TRUE(is_interpreted_format_str_as_expected("2c5*", "cc*****"));  // Repeated 'c' and '*'
    CU_ASSERT_TRUE(is_interpreted_format_str_as_expected("10i", "iiiiiiiiii")); // Repeated 'i'

    // Edge cases
    CU_ASSERT_TRUE(is_interpreted_format_str_as_expected("0c", ""));            // Zero repetition
    CU_ASSERT_TRUE(is_interpreted_format_str_as_expected("", ""));              // Empty string
    CU_ASSERT_TRUE(is_interpreted_format_str_as_expected("**", "**"));          // No formatting, plain string

    // Invalid cases (handle gracefully or document expected behavior)
    CU_ASSERT_TRUE(is_interpreted_format_str_as_expected("3x", ""));            // Unsupported character 'x'
    CU_ASSERT_TRUE(is_interpreted_format_str_as_expected("2i*", "ii*"));        // Correct interpretation with additional characters

    // Stress test
    CU_ASSERT_TRUE(is_interpreted_format_str_as_expected("20c", "cccccccccccccccccccc"));

    // Whitespace handling
    CU_ASSERT_TRUE(is_interpreted_format_str_as_expected(" 3i ", "iii"));       // Ignore surrounding spaces
}

int main()
{
    // Initialize CUnit test registry
    if (CU_initialize_registry() != CUE_SUCCESS)
        return CU_get_error();

    // Init test suite
    // Add a suite for the bits_required_for_layout tests
    CU_pSuite format_string_test_suite = CU_add_suite("Format str test suite", init_suite, clean_suite);
    if (format_string_test_suite == NULL)
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // Individual test cases
    // Add individual test cases to the suite
    if (NULL == CU_add_test(format_string_test_suite, "test interpret string", test_interpret_stringformat))
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

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
#include "../../src/header/bit_manip.h"

int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }


struct test_info {
    uint64_t numeric_representation;
    char* char_representation;
};
/**** Some Samples for the tests ****/
struct test_info tests[] = {
    {0x0, "00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000"},
    {0xFFFFFFFFFFFFFFFF, "11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111"},
    {0xAAAAAAAAAAAAAAAA, "10101010 10101010 10101010 10101010 10101010 10101010 10101010 10101010"},
    {0x00000000FFFFFFFF, "00000000 00000000 00000000 00000000 11111111 11111111 11111111 11111111"},
    {0xF0F0F0F0F0F0F0F0, "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000"}
};


void test_bits_to_bitstring() {
    for (int i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        char* result = bits_to_bitstring(tests[i].numeric_representation);

        // Let's verify that we have an allocated string
        CU_ASSERT_PTR_NOT_NULL(result);
        if (result == NULL) {
            printf("Test %d failed: Memory allocation error\n", i + 1);
            continue;
        }

        //let's compare them
        bool equal = strcmp(result, tests[i].char_representation) == 0;
        CU_ASSERT(equal);
        if (!equal) {
            // Test failed, lets print some nice information
            printf("Test %d failed:\n", i + 1);
            printf("Input:    0x%016llX\n", tests[i].numeric_representation);
            printf("Expected: %s\n", tests[i].char_representation);
            printf("Got:      %s\n", result);
        }
        //lets not crash our program
        if(result != NULL) {
            free(result);
        }
    }
}

void test_bitstring_to_bits() {
    for (int i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        uint64_t result = bitstring_to_bits(tests[i].char_representation);

        //let's compare them
        bool equal = result == tests[i].numeric_representation;

        //assert equality
        CU_ASSERT(equal);
        if (!equal) {
            // Test failed, lets print some nice information
            printf("Test %d failed:\n", i + 1);
            printf("Input:    %s\n", tests[i].char_representation);
            printf("Expected: 0x%016llX\n", tests[i].numeric_representation);
            printf("Got:      0x%016llX\n", result);
        }
    }
}

void test_reverse_bits() {
    struct test_info reversed_tests[] = {
        {0x0, "00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000"},
        {0xFFFFFFFFFFFFFFFF, "11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111"},
        {0x5555555555555555, "01010101 01010101 01010101 01010101 01010101 01010101 01010101 01010101"},
        {0xFFFFFFFF00000000, "11111111 11111111 11111111 11111111 00000000 00000000 00000000 00000000"},
        {0xF0F0F0F0F0F0F0F, "00001111 00001111 00001111 00001111 00001111 00001111 00001111 00001111"}
    };

    //lets walk through the whole index of tests
    for (int i = 0; i < sizeof(reversed_tests) / sizeof(reversed_tests[0]); i++) {

        //get reversed bits
        uint64_t reversed_bits = reverse_bits(tests[i].numeric_representation);

        // Compare reversed numeric value
        bool numeric_equal = (reversed_bits == reversed_tests[i].numeric_representation);
        CU_ASSERT(numeric_equal);

        // if fail as usual priont nice debug messages
        if (!numeric_equal) {
            printf("Test %d failed (Numeric mismatch):\n", i + 1);
            printf("Input:    0x%016llX\n", tests[i].numeric_representation);
            printf("Expected: 0x%016llX\n", reversed_tests[i].numeric_representation);
            printf("Got:      0x%016llX\n", reversed_bits);
        }

        // Let's use our fancy to sring method now and make sure we get the correct answer
        char* reversed_str = bits_to_bitstring(reversed_bits);

        // Compare string representation
        bool string_equal = strcmp(reversed_str, reversed_tests[i].char_representation) == 0;
        CU_ASSERT(string_equal);
        if (!string_equal) {
            printf("Test %d failed (String mismatch):\n", i + 1);
            printf("Input:    0x%016llX\n", tests[i].numeric_representation);
            printf("Expected: %s\n", reversed_tests[i].char_representation);
            printf("Got:      %s\n", reversed_str);
        }

        if (reversed_str != NULL) {
            free(reversed_str);
        }
    }

}

void test_set_upper_4bytes() {

    // A set where lower byte is same but the upper is changed, so if the function works
    // we should be able to set the upper to : 10010010 10000001 10000001 10001000
    // in the original test and match with the one from changed upper
    struct test_info changed_upper_tests[] = {
        {0x9281818800000000, "10010010 10000001 10000001 10001000 00000000 00000000 00000000 00000000"},
        {0x92818188FFFFFFFF, "10010010 10000001 10000001 10001000 11111111 11111111 11111111 11111111"},
        {0x92818188AAAAAAAA, "10010010 10000001 10000001 10001000 10101010 10101010 10101010 10101010"},
        {0x92818188FFFFFFFF, "10010010 10000001 10000001 10001000 11111111 11111111 11111111 11111111"},
        {0x92818188F0F0F0F0, "10010010 10000001 10000001 10001000 11110000 11110000 11110000 11110000"}
    };

    uint32_t source = 0x92818188;  //10010010 10000001 10000001 10001000  in hexadecimal

    for (int i = 0; i < sizeof(changed_upper_tests) / sizeof(changed_upper_tests[0]); i++) {
        uint64_t result = set_upper_4bytes(tests[i].numeric_representation, source);

        //Compare numerically
        bool numeric_equal = result == changed_upper_tests[i].numeric_representation;
        CU_ASSERT(numeric_equal);

        //Print nice error msg
        if (!numeric_equal) {
            printf("Test %d failed (Numeric mismatch):\n", i + 1);
            printf("Input:    Lower: 0x%08X, Upper: 0x%08X\n",
                   (uint32_t)(tests[i].numeric_representation & 0xFFFFFFFF), source);
            printf("Expected: 0x%016llX\n", changed_upper_tests[i].numeric_representation);
            printf("Got:      0x%016llX\n", result);
        }

        // Compare string as usual
        char* result_str = bits_to_bitstring(result);
        bool string_equal = strcmp(result_str, changed_upper_tests[i].char_representation) == 0;
        CU_ASSERT(string_equal);

        if (!string_equal) {
            printf("Test %d failed (String mismatch):\n", i + 1);
            printf("Input:    Lower: 0x%08X, Upper: 0x%08X\n",
                   (uint32_t)(tests[i].numeric_representation & 0xFFFFFFFF), source);
            printf("Expected: %s\n", changed_upper_tests[i].char_representation);
            printf("Got:      %s\n", result_str);
        }

        if (result_str != NULL) {
            free(result_str);
        }
    }
}


void test_set_lower_4bytes() {
    // A set where upper byte is the same but the lower is changed.
    // If the function works, we should be able to set the lower to: 10001010 01010011 01010011 01111111
    // in the original test and match with the one from changed lower.
    struct test_info changed_lower_tests[] = {
        {0x8A53537F, "00000000 00000000 00000000 00000000 10001010 01010011 01010011 01111111"},
        {0xFFFFFFFF8A53537F, "11111111 11111111 11111111 11111111 10001010 01010011 01010011 01111111"},
        {0xAAAAAAAA8A53537F, "10101010 10101010 10101010 10101010 10001010 01010011 01010011 01111111"},
        {0x000000008A53537F, "00000000 00000000 00000000 00000000 10001010 01010011 01010011 01111111"},
        {0xF0F0F0F08A53537F, "11110000 11110000 11110000 11110000 10001010 01010011 01010011 01111111"}
    };

    uint32_t source = 0x8A53537F;  // 10001010 01010011 01010011 01111111 in hexadecimal

    for (int i = 0; i < sizeof(changed_lower_tests) / sizeof(changed_lower_tests[0]); i++) {
         uint64_t result = set_lower_4bytes(tests[i].numeric_representation, source);

        // Compare numerically
        bool numeric_equal = result == changed_lower_tests[i].numeric_representation;
        CU_ASSERT(numeric_equal);

        // Print nice error message
        if (!numeric_equal) {
            printf("Test %d failed (Numeric mismatch):\n", i + 1);
            printf("Input:    Upper: 0x%08X, Lower: 0x%08X\n",
                   (uint32_t)(tests[i].numeric_representation >> 32), source);
            printf("Expected: 0x%016llX\n", changed_lower_tests[i].numeric_representation);
            printf("Got:      0x%016llX\n", result);
        }

        // Compare string as usual
        char* result_str = bits_to_bitstring(result);
        bool string_equal = strcmp(result_str, changed_lower_tests[i].char_representation) == 0;
        CU_ASSERT(string_equal);

        if (!string_equal) {
            printf("Test %d failed (String missmatch):\n", i + 1);
            printf("input:    Upper: 0x%08X, Lower: 0x%08X\n",
                   (uint32_t)(tests[i].numeric_representation >> 32), source);
            printf("Expected: %s\n", changed_lower_tests[i].char_representation);
            printf("got:      %s\n", result_str);
        }

        if (result_str != NULL) {
            free(result_str);
        }
    }
}



// calls copy_bits_from_msb(uint64_t dest, uint64_t source, uint64_t number_of_bits)
void test_copy_bits_from_msb(){
    struct test_info dest_test[] = {
        {0x0, "00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000"},
        {0xFFFFFFFFFFFFFFFF, "11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111"},
        {0xAAAAAAAAAAAAAAAA, "10101010 10101010 10101010 10101010 10101010 10101010 10101010 10101010"},
        {0x00000000FFFFFFFF, "00000000 00000000 00000000 00000000 11111111 11111111 11111111 11111111"},
        {0xF0F0F0F0F0F0F0F0, "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000"}
    };
    struct test_info modification_tests[] = {
        {0x00FFFFFFFFFFFFFF, "00000000 11111111 11111111 11111111 11111111 11111111 11111111 11111111"},
        {0xFF00FFFFFFFFFF00, "11111111 00000000 11111111 11111111 11111111 11111111 11111111 00000000"},
        {0x0000FFFFFFFFFF00, "00000000 00000000 11111111 11111111 11111111 11111111 11111111 00000000" }
    };


    /** Testing a full 64 bit copy **/
    // Test 1, using index 2 as source and number_of_bits set to 64(all)
    // 1. if dest is an empty uint64_t should still become identical to index 2, (since we copy all)
    // 2. if dest is index 4 should still become identical to index 2, (since we copy all)
    uint64_t dest = dest_test[0].numeric_representation; // Empty
    uint64_t source = modification_tests[2].numeric_representation; // Index 2
    uint64_t number_of_bits = 64; // Full copy

    //copy to empty dest
    dest = copy_bits_from_msb(dest, source, number_of_bits);
    CU_ASSERT(dest == source);
    if (dest != source) {
        printf("Test msb to empty failed\n");
        printf("Expected: %016llX\n", source);
        printf("got:      %016llX\n", dest);
    }

    //copy to non-empty dest
    dest = dest_test[2].numeric_representation; // Not empty
    dest = copy_bits_from_msb(dest, source, number_of_bits);
    CU_ASSERT(dest == source);
    if (dest != source) {
        printf("Test msb to non-empty failed\n");
        printf("Expected: %016llX\n", source);
        printf("got:      %016llX\n", dest);
    }

    /** Partial copy test **/
    //Copying 8 bits from index 1 to index 0 should create something equal to index 2
    dest = modification_tests[1].numeric_representation;
    source = modification_tests[0].numeric_representation;
    number_of_bits = 8;

    dest = copy_bits_from_msb(dest, source, number_of_bits);

    uint64_t expected = 0x0000FFFFFFFFFF00;
    CU_ASSERT(dest == expected);

    if (dest != expected) {
        printf("Test msb partial copy failed\n");
        printf("Expected: %016llX\n", expected);
        printf("got:      %016llX\n", dest);
    }
}

// calls copy_bits_from_lsb(uint64_t dest, uint64_t source, uint64_t number_of_bits)
void test_copy_bits_from_lsb(){
    struct test_info dest_test[] = {
        {0x0, "00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000"},
        {0xFFFFFFFFFFFFFFFF, "11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111"},
        {0xAAAAAAAAAAAAAAAA, "10101010 10101010 10101010 10101010 10101010 10101010 10101010 10101010"},
        {0x00000000FFFFFFFF, "00000000 00000000 00000000 00000000 11111111 11111111 11111111 11111111"},
        {0xF0F0F0F0F0F0F0F0, "11110000 11110000 11110000 11110000 11110000 11110000 11110000 11110000"}
    };
    struct test_info modification_tests[] = {
        {0xFFFFFFFF00FFFF00, "11111111 11111111 11111111 11111111 00000000 11111111 11111111 00000000"},
        {0xFFFFFFFFFF00FFFF, "11111111 11111111 11111111 11111111 11111111 00000000 11111111 11111111"},
        {0xFFFFFFFF0000FFFF, "11111111 11111111 11111111 11111111 00000000 00000000 11111111 11111111" }
    };
    /** Testing a full copy **/
    // Test 1, using index 2 as source and number_of_bits set to 64(all)
    // 1. if dest is an empty uint64_t should still become identical to index 2, (since we copy all)
    // 2. if dest is index 4 should still become identical to index 2, (since we copy all)

    // 64 bit copy top empty location
    uint64_t dest = dest_test[0].numeric_representation;
    uint64_t source = modification_tests[2].numeric_representation;
    uint64_t number_of_bits = 64;
    dest = copy_bits_from_lsb(dest, source, number_of_bits);
    CU_ASSERT(dest == source);
    if (dest != source) {
        printf("Test 3 (LSB to empty) failed\n");
        printf("Expected: %016llX\n", source);
        printf("got:      %016llX\n", dest);
    }

    // 64 bit copy to non empty location
    dest = dest_test[2].numeric_representation;
    dest = copy_bits_from_lsb(dest, source, number_of_bits);
    CU_ASSERT(dest == source);
    if (dest != source) {
        printf("Test 4 (LSB to non-empty) failed\n");
        printf("Expected: %016llX\n", source);
        printf("got:      %016llX\n", dest);
    }

    //copying 24 bits from index 1 to index 0 should create something equal to index 2
    // Copy 24 bits from index 1 to empty destination
    dest = modification_tests[0].numeric_representation;
    source = modification_tests[1].numeric_representation;
    number_of_bits = 24;

    dest = copy_bits_from_lsb(dest, source, number_of_bits);

    uint64_t expected = 0xFFFFFFFF0000FFFF;
    CU_ASSERT(dest == expected);

    if (dest != expected) {
        printf("Test LSB partial copy 24 bits failed\n");
        printf("Expected: %016llX\n", expected);
        printf("Got:      %016llX\n", dest);
    }

}

void test_mask_from_lsb(){
   uint64_t src = 0xFFFFFFFFFFFFFFFF;
   uint64_t mask4 = 0xFFFFFFFFFFFFFFF0;
   uint64_t mask8 = 0xFFFFFFFFFFFFFF00;
   uint64_t mask32 = 0xFFFFFFFF00000000;
   CU_ASSERT_EQUAL(mask_from_lsb(src, 4), mask4);
   CU_ASSERT_EQUAL(mask_from_lsb(src, 8), mask8);
   CU_ASSERT_EQUAL(mask_from_lsb(src, 32), mask32);
}

void test_mask_from_msb(){
    uint64_t src = 0xFFFFFFFFFFFFFFFF;
    uint64_t mask4 = 0x0FFFFFFFFFFFFFFF;
    uint64_t mask8 = 0x00FFFFFFFFFFFFFF;
    uint64_t mask32 = 0x00000000FFFFFFFF;
    CU_ASSERT_EQUAL(mask_from_msb(src, 4), mask4);
    CU_ASSERT_EQUAL(mask_from_msb(src, 8), mask8);
    CU_ASSERT_EQUAL(mask_from_msb(src, 32), mask32);
}

int main() {
    if (CU_initialize_registry() != CUE_SUCCESS)
        return CU_get_error();

    // Add test suite
    CU_pSuite bit_vector_test_suite = CU_add_suite("Bit Vector Test Suite", init_suite, clean_suite);
    if (!bit_vector_test_suite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // Add individual test cases
    if (
        (NULL == CU_add_test(bit_vector_test_suite, "Test bits to bitstring", test_bits_to_bitstring)) ||
        (NULL == CU_add_test(bit_vector_test_suite, "Test bitstring to bits", test_bitstring_to_bits)) ||
        (NULL == CU_add_test(bit_vector_test_suite, "Test reverse bits", test_reverse_bits)) ||
        (NULL == CU_add_test(bit_vector_test_suite, "Test set upper 4 bytes", test_set_upper_4bytes)) ||
        (NULL == CU_add_test(bit_vector_test_suite, "Test set lower 4 bytes", test_set_lower_4bytes)) ||
        (NULL == CU_add_test(bit_vector_test_suite, "Test copy bits from source to dest respecting lsb", test_copy_bits_from_lsb)) ||
        (NULL == CU_add_test(bit_vector_test_suite, "Test copy bits from source to dest respecting msb", test_copy_bits_from_msb)) ||
        (NULL == CU_add_test(bit_vector_test_suite, "Test masking from lsb", test_mask_from_lsb)) ||
        (NULL == CU_add_test(bit_vector_test_suite, "Test masking from msb", test_mask_from_msb))
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

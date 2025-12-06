#pragma once
#include <stdbool.h>
#include <string.h>
#include <wchar.h>
#include "../../src/gc.h"

typedef union elem elem_t;

union elem {
	int number;
	char *string;
	wchar_t *string_long;
	unsigned int number_unsigned;
	bool boolean;
	float number_float;
	void *any;
};

/**
 * @file hash_table.h
 * @author Åke Lindström
 * @date 9 Oct 2022
 * @brief common functions used by the hash table and linked list structure
 *
 * This file contains the defenitions of compare functions for
 * different datatypes. It also contains hash functions for 
 * different datatypes for the hash table data structure
 * and predicate functions.
 *
 * @see $CANVAS_OBJECT_REFERENCE$/assignments/gb54499f3b7b264e3af3b68c756090f52
 */

typedef union elem ioopm_elem_t;
typedef bool ioopm_cmp_t(ioopm_elem_t v1,ioopm_elem_t v2);

/// @brief compare function for integers
/// @param value1 first value to be compared
/// @param value2 second value to be compared
/// @return true if the values match and false otherwise
bool compare_int(ioopm_elem_t value1, ioopm_elem_t value2);

/// @brief compare function for unsigned integers
/// @param value1 first value to be compared
/// @param value2 second value to be compared
/// @return true if the values match and false otherwise
bool compare_uint(ioopm_elem_t value1, ioopm_elem_t value2);

/// @brief compare function for boolean
/// @param value1 first value to be compared
/// @param value2 second value to be compared
/// @return true if the values match and false otherwise
bool compare_bool(ioopm_elem_t value1, ioopm_elem_t value2);

/// @brief compare function for floats
/// @param value1 first value to be compared
/// @param value2 second value to be compared
/// @return true if the values match and false otherwise
bool compare_float(ioopm_elem_t value1, ioopm_elem_t value2);

/// @brief compare function for strings
/// @param value1 first value to be compared
/// @param value2 second value to be compared
/// @return true if the values match and false otherwise
bool compare_string(ioopm_elem_t value1, ioopm_elem_t value2);

/// @brief compare function for long strings
/// @param value1 first value to be compared
/// @param value2 second value to be compared
/// @return true if the values match and false otherwise
bool compare_string_long(ioopm_elem_t value1, ioopm_elem_t value2);

/// @brief hash integers to the hash table
/// @param key the key to be hashed
/// @param capacity the numbers of buckets currently in the hash table
/// @return the hash value
int hash_int(ioopm_elem_t key, int capacity);

/// @brief hash the sum of a string
/// @param key the key to be hashed
/// @param capacity the numbers of buckets currently in the hash table
/// @return the hash value
int hash_string_sum(ioopm_elem_t key, int capacity);

/// @brief hash the sum of a long string
/// @param key the key to be hashed
/// @param capacity the numbers of buckets currently in the hash table
/// @return the hash value
int hash_string_sum_long(ioopm_elem_t key, int capacity);

/// @brief predicate function for matching values
/// @param cmp_ignored compare function for keys not to be used
/// @param cmp the compare function for the values
/// @param key the key for the entry
/// @param value the value for the entry
/// @param x void pointer to the value to be compared
/// @return the hash value
bool value_equiv(ioopm_cmp_t *cmp_ignored,ioopm_cmp_t *cmp,ioopm_elem_t key,ioopm_elem_t value, void *x);


/// ADDED
/// @brief A strdup equivalent for the gc project
/// @param str the src string to copy
/// @param h the heap to allocate to
/// @return the eqvivalent string from the new location
char *h_strdup(const char *str, heap_t *h);

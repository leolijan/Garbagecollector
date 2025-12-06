#include "common.h"
#include <string.h>
#include <stdlib.h>
#include "../../src/gc.h"

bool compare_int(ioopm_elem_t value1, ioopm_elem_t value2) {
    return value1.number == value2.number;
}

bool compare_float(ioopm_elem_t value1, ioopm_elem_t value2) {
    return value1.number_float == value2.number_float;
}

bool compare_bool(ioopm_elem_t value1, ioopm_elem_t value2) {
    return value1.boolean == value2.boolean;
}

bool compare_string(ioopm_elem_t value1, ioopm_elem_t value2) {
	return strcmp(value1.string,value2.string) == 0;
}

bool compare_string_long(ioopm_elem_t value1, ioopm_elem_t value2) {
	return wcscmp(value1.string_long,value2.string_long) == 0;
}

int hash_int(ioopm_elem_t key,int capacity) {
	int index = key.number % capacity;
	if(index < 0) {
		index += capacity;
	}
	return index;
}

int hash_string_sum(ioopm_elem_t key, int capacity) {
	char *str = key.string;
	int result = 0;
	do {
		result += *str;
	}
	while (*++str != '\0');

	result = abs(result);
	
	result = result % capacity;
	return result;
}

int hash_string_sum_long(ioopm_elem_t key, int capacity) {


	wchar_t *str = key.string_long;
	int result = 0;
	do {
		result += *str;
	}
	while (*++str != '\0');
	return result % capacity;
}

//predicate function
bool value_equiv(ioopm_cmp_t *cmp_ignored,ioopm_cmp_t *cmp,ioopm_elem_t key, ioopm_elem_t value, void *x) {
	ioopm_elem_t *other_value_ptr = x;
	ioopm_elem_t other_value = *other_value_ptr;
	return cmp(value,other_value);
}

char *h_strdup(const char *str, heap_t *h) {
    size_t len = strlen(str) + 1;  // +1 for null terminator
    char *new_str = h_alloc_raw(h, len * sizeof(char));  // Allocate memory for the string
    if (new_str != NULL) {
        strcpy(new_str, str);  // Copy the string into the allocated memory
    }
    return new_str;
}
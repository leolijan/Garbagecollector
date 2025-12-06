#pragma once

#ifndef HASH_TABLE_DYNAMIC_H
#define HASH_TABLE_DYNAMIC_H

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"
#include "linked_list.h"
#include "common.h"
#include "../../src/gc.h"

#define No_Buckets 17

/**
 * @file hash_table.h
 * @author Åke Lindström
 * @date 1 Sep 2022
 * @brief Simple hash table that maps integer keys to string values.
 *
 * Here typically goes a more extensive explanation of what the header
 * defines. Doxygens tags are words preceeded by either a backslash @\
 * or by an at symbol nnnn@@.
 *
 * @see $CANVAS_OBJECT_REFERENCE$/assignments/gb54499f3b7b264e3af3b68c756090f52
 */


typedef struct entry ioopm_entry_t;
//typedef struct elem elem_t;

typedef bool ioopm_predicate(ioopm_cmp_t *cmp, ioopm_cmp_t *cmp_value, ioopm_elem_t key, ioopm_elem_t value, void *extra);

typedef void ioopm_apply_function(ioopm_elem_t key,  ioopm_elem_t *value, void *extra);

typedef struct hash_table ioopm_hash_table_t;

typedef int ioopm_hash_fun(ioopm_elem_t value, int capacity);

ioopm_hash_table_t *ioopm_hash_table_create_dynamic(ioopm_hash_fun *fun, ioopm_cmp_t *cmp, ioopm_cmp_t *cmp_value, size_t capacity, float load_factor, heap_t *h);

/// @brief Create a new hash table
/// @return A new empty hash table
ioopm_hash_table_t *ioopm_hash_table_create(ioopm_hash_fun *fun, ioopm_cmp_t *cmp, ioopm_cmp_t *cmp_value, heap_t *h);

/// @brief add key => value entry in hash table ht
/// @param ht hash table operated upon
/// @param key key to insert
/// @param value value to insert
void ioopm_hash_table_insert(ioopm_hash_table_t *ht, ioopm_elem_t key, ioopm_elem_t value, heap_t *h);

/// @brief lookup value for key in hash table ht
/// @param ht hash table operated upon
/// @param key key to lookup
/// @param result pointer to a string that changes to the value if it exists
/// @return true if the value exists on the given key, otherwise false
ioopm_elem_t ioopm_hash_table_lookup(ioopm_hash_table_t *ht, ioopm_elem_t key);

/// @brief remove any mapping from key to a value
/// @param ht hash table operated upon
/// @param key key to remove
/// @return the value mapped to by key if it exists
ioopm_elem_t ioopm_hash_table_remove(ioopm_hash_table_t *ht, ioopm_elem_t key);

//Utility functions!

/// @brief returns the number of key => value entries in the hash table
/// @param h hash table operated upon
/// @return the number of key => value entries in the hash table
size_t ioopm_hash_table_size(ioopm_hash_table_t *ht);

/// @brief checks if the hash table is empty
/// @param h hash table operated upon
/// @return true is size == 0, else false
bool ioopm_hash_table_is_empty(ioopm_hash_table_t *ht);

/// @brief clear all the entries in a hash table
/// @param h hash table operated upon
void ioopm_hash_table_clear(ioopm_hash_table_t *ht);

/// @brief return the keys for all entries in a hash map (in no particular order, but same as ioopm_hash_table_values)
/// @param h hash table operated upon
/// @return an array of keys for hash table h
ioopm_list_t *ioopm_hash_table_keys(ioopm_hash_table_t *ht, heap_t *h);

/// @brief return the values for all entries in a hash map (in no particular order, but same as ioopm_hash_table_keys)
/// @param h hash table operated upon
/// @return an array of values for hash table h
ioopm_list_t *ioopm_hash_table_values(ioopm_hash_table_t *ht, heap_t *h);

/// @brief check if a hash table has an entry with a given key
/// @param h hash table operated upon
/// @param key the key sought
bool ioopm_hash_table_has_key(ioopm_hash_table_t *ht, ioopm_elem_t key);

/// @brief check if a hash table has an entry with a given value
/// @param h hash table operated upon
/// @param value the value sought
bool ioopm_hash_table_has_value(ioopm_hash_table_t *ht, ioopm_elem_t value);



/// @brief check if a predicate is satisfied by all entries in a hash table
/// @param h hash table operated upon
/// @param pred the predicate
/// @param arg extra argument to pred
bool ioopm_hash_table_all(ioopm_hash_table_t *ht, ioopm_predicate *pred, void *arg);

/// @brief check if a predicate is satisfied by any entry in a hash table
/// @param h hash table operated upon
/// @param pred the predicate
/// @param arg extra argument to pred
bool ioopm_hash_table_any(ioopm_hash_table_t *ht, ioopm_predicate *pred, void *arg);

/// @brief apply a function to all entries in a hash table
/// @param h hash table operated upon
/// @param apply_fun the function to be applied to all elements
/// @param arg extra argument to apply_fun
void ioopm_hash_table_apply_to_all(ioopm_hash_table_t *ht, ioopm_apply_function *apply_fun, void *arg);

#endif

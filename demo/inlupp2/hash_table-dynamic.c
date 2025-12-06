#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "hash_table-dynamic.h"
#include "linked_list.h"
#include "../../src/gc.h"

struct entry {
	ioopm_elem_t key;
	ioopm_elem_t value;
	ioopm_entry_t *next;
};

struct hash_table {
	ioopm_entry_t **buckets;
	size_t size;
	size_t capacity;
	float load_factor;
	ioopm_hash_fun *hash_function;
	ioopm_cmp_t *cmp;
	ioopm_cmp_t *cmp_value;
};

ioopm_hash_table_t *ioopm_hash_table_create_dynamic(ioopm_hash_fun *fun, ioopm_cmp_t *cmp, ioopm_cmp_t *cmp_value, size_t capacity, float load_factor, heap_t *h) {
	char *hash_table_fs = "*2lf3*"; /// Modified
	ioopm_hash_table_t *result = h_alloc_struct(h, hash_table_fs); /// Modified
	result->size = 0;
	result->hash_function = fun;
	result->cmp = cmp;
	result->cmp_value = cmp_value;

	result->capacity = capacity;
	result->load_factor = load_factor;

	char *buckets_fs = "17*"; /// Modified
	ioopm_entry_t **buckets = h_alloc_struct(h, buckets_fs); /// Modified
	result->buckets = buckets;

	/// Nollställ? Modified
	for (size_t i = 0; i < result->capacity; i++) {
    	result->buckets[i] = NULL;
	}
	return result;
}

ioopm_hash_table_t *ioopm_hash_table_create(ioopm_hash_fun *fun, ioopm_cmp_t *cmp, ioopm_cmp_t *cmp_value, heap_t *h) {
	return ioopm_hash_table_create_dynamic(fun,cmp,cmp_value,No_Buckets,0.75,h);
}

static bool key_equiv(ioopm_cmp_t *cmp, ioopm_cmp_t *cmp_ignored, ioopm_elem_t key, ioopm_elem_t value_ignored, void *x) {
	ioopm_elem_t *other_key_ptr = x;
	ioopm_elem_t other_key = *other_key_ptr;
	return cmp(key,other_key);
}

static ioopm_entry_t **find_previous_entry_for_key(ioopm_entry_t **entry, ioopm_elem_t key, ioopm_hash_table_t *ht) {
	ioopm_entry_t **prev = NULL;
	if (entry == NULL) {
		return NULL;
	}
	ioopm_entry_t **current = entry;

	while (*current != NULL) {
		if (ht->cmp((*current)->key, key)) {
			return prev;
		}
		prev = current;
		current = &(*current)->next;
	}
	return NULL;
}

static ioopm_entry_t *entry_create(ioopm_elem_t key, ioopm_elem_t value, ioopm_entry_t *ent, heap_t *h) {
	/// 3* because elem_t largest is a pointer 8 bytes.
	char *entry_fs = "3*"; /// Modified
	ioopm_entry_t *entry = h_alloc_struct(h, entry_fs); /// Modified
	entry->key = key;
	entry->value = value;
	entry->next = ent;
	return entry;
}

static size_t find_next_capacity(size_t capacity) {
    size_t primes[] = {17, 31, 67, 127, 257, 509, 1021, 2053, 4099, 8191, 16381};
    size_t num_primes = sizeof(primes) / sizeof(primes[0]);

    for (size_t i = 0; i < num_primes; i++) {
        if (primes[i] > capacity) {
            return primes[i];
        }
    }
    return primes[num_primes - 1];
}

void hash_table_grow(ioopm_hash_table_t *ht, heap_t *h) {
    size_t new_capacity = find_next_capacity(ht->capacity);
	char *entry_fs = "3*"; /// Modified
    ioopm_entry_t **new_buckets = h_alloc_struct(h, entry_fs); /// Modified

    for (size_t i = 0; i < ht->capacity; i++) {
        ioopm_entry_t *entry = ht->buckets[i];
        while (entry != NULL) {
            size_t new_index = ht->hash_function(entry->key, new_capacity);            
			ioopm_entry_t *tmp = entry;
			entry = entry->next;
			tmp->next = new_buckets[new_index];
			new_buckets[new_index] = tmp;
        }
    }
	
	ht->capacity = new_capacity;
    ht->buckets = new_buckets;
}

void ioopm_hash_table_insert(ioopm_hash_table_t *ht, ioopm_elem_t key, ioopm_elem_t value, heap_t *h) {

	//Check to see if the hash table needs to grow
	if ((float)(ht->size + 1) / ht->capacity >= ht->load_factor) {
		hash_table_grow(ht, h);
	}
  
	size_t bucket = ht->hash_function(key, ht->capacity);  
	ioopm_entry_t **prev = find_previous_entry_for_key(&ht->buckets[bucket], key, ht);
	ioopm_entry_t *current = prev == NULL ? ht->buckets[bucket] : (*prev)->next;

    if (current != NULL && ht->cmp(current->key, key)) {
        current->value = value;
    } else {
        ioopm_entry_t *new_entry = entry_create(key, value, current, h);
        if (prev == NULL) {
            ht->buckets[bucket] = new_entry;
        } else {
            (*prev)->next = new_entry;
        }
        ht->size += 1;
    }
}

ioopm_elem_t ioopm_hash_table_remove(ioopm_hash_table_t *ht, ioopm_elem_t key) {

	size_t bucket = ht->hash_function(key, ht->capacity);

	if (ioopm_hash_table_lookup(ht, key).string != NULL) {
		ht->size--;
		ioopm_entry_t **prev = find_previous_entry_for_key(&ht->buckets[bucket], key, ht);

		//There is no previous value.
		if (prev == NULL) {
			if (ht->buckets[bucket] && ht->cmp(ht->buckets[bucket]->key, key)) {
				if (ht->buckets[bucket]->next != NULL) {
					ioopm_entry_t *remove = ht->buckets[bucket];
					ht->buckets[bucket] = ht->buckets[bucket]->next;
					ioopm_elem_t val_removed = remove->value;
					return val_removed;
				}else {
					ioopm_entry_t *remove = ht->buckets[bucket];
					ht->buckets[bucket] = NULL;
					ioopm_elem_t val_removed = remove->value;
					return val_removed;
				}
			} else {
				return (ioopm_elem_t){.string = NULL};
			}
		} else {
			ioopm_entry_t *to_remove = (*prev)->next;
			(*prev)->next = to_remove->next;
			ioopm_elem_t value_removed = to_remove->value;
			return value_removed;
		}
	}
	return (ioopm_elem_t){.string = NULL};
}

ioopm_elem_t ioopm_hash_table_lookup(ioopm_hash_table_t *ht, ioopm_elem_t key) {
	size_t bucket = ht->hash_function(key, ht->capacity);

	if (ht->buckets[bucket] == NULL) {
		return (ioopm_elem_t){.string = NULL};
	}

	ioopm_entry_t **tmp = find_previous_entry_for_key(&ht->buckets[bucket], key, ht);

	if (tmp == NULL && ht->cmp(ht->buckets[bucket]->key, key)) {
		return ht->buckets[bucket]->value;
	}

	if (tmp != NULL) {
		ioopm_entry_t *current = (*tmp)->next;

		if (current != NULL) {
			if (ht->cmp(current->key, key)) {
				return current->value;
			}
		}
	}
	return (ioopm_elem_t){.string = NULL};
}

size_t ioopm_hash_table_size(ioopm_hash_table_t *ht) {
	return ht->size;
}

bool ioopm_hash_table_is_empty(ioopm_hash_table_t *ht) {
	return ht->size == 0;
}

void ioopm_hash_table_clear(ioopm_hash_table_t *ht) {
	for (int i = 0; i < ht->capacity; i++) {
		ht->buckets[i] = NULL;
		ht->size = 0;
	}
}

ioopm_list_t *ioopm_hash_table_keys(ioopm_hash_table_t *ht, heap_t *h) {
	ioopm_list_t *keys = ioopm_linked_list_create(compare_int, h);
		
	for(size_t i = 0; i < ht->capacity; i++) {
		ioopm_entry_t *bucket = ht->buckets[i];
		while(bucket != NULL) {
			ioopm_linked_list_append(keys,bucket->key, h);
			bucket = bucket->next;
		}
	}
	return keys;
}

ioopm_list_t *ioopm_hash_table_values(ioopm_hash_table_t *ht, heap_t *h) {
	ioopm_list_t *values = ioopm_linked_list_create(compare_string, h);

	for(size_t i = 0; i < ht->capacity; i++) {
		ioopm_entry_t *bucket = ht->buckets[i];
		while(bucket != NULL) {
			ioopm_linked_list_append(values,(ioopm_elem_t)bucket->value, h);
			bucket = bucket->next;
		}
	}

	return values;
}

bool ioopm_hash_table_any(ioopm_hash_table_t *ht, ioopm_predicate *pred, void *arg) {
	for (size_t i = 0; i < ht->capacity; i++) {
		ioopm_entry_t *entry = ht->buckets[i];
		while (entry != NULL) {
			if (pred(ht->cmp,ht->cmp_value,entry->key, entry->value, arg)){
				return true;
			}
			entry = entry->next;
		}
	}
	return false;
}

bool ioopm_hash_table_has_key(ioopm_hash_table_t *ht, ioopm_elem_t key) {
	return ioopm_hash_table_any(ht, key_equiv, &key);
}

bool ioopm_hash_table_has_value(ioopm_hash_table_t *ht, ioopm_elem_t value) {
	return ioopm_hash_table_any(ht, &value_equiv, &value);
}

bool ioopm_hash_table_all(ioopm_hash_table_t *ht, ioopm_predicate *pred, void *arg) {
	for(int i = 0; i < ht->capacity; i++) {
		ioopm_entry_t *bucket = ht->buckets[i];
		while(bucket != NULL) {
			if(!pred(ht->cmp,ht->cmp_value,bucket->key,bucket->value,&arg)){
				return false;
			}
			bucket = bucket->next;
		}
	}
	return true;
}

void ioopm_hash_table_apply_to_all(ioopm_hash_table_t *ht, ioopm_apply_function *apply_fun, void *arg) {
	for(size_t i = 0; i < ht->capacity; i++) {
		ioopm_entry_t *bucket = ht->buckets[i];
		while(bucket != NULL) {
			apply_fun((ioopm_elem_t)bucket->key,&bucket->value,&arg);			
			bucket = bucket->next;
		}
	}
}

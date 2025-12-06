#ifndef __TRAVERSE_STACK_H__
#define __TRAVERSE_STACK_H__

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <setjmp.h>
#include <stdlib.h>
#include "gc.h"

void **root_ptrs(heap_t *h, size_t *count);
void invalidate_ptrs_at_heap(heap_t *h, void *dbg_value);
bool validate_ptr(heap_t *h, void* ptr);

//returns null if not found!
void* find_header_address_through_footer(heap_t *h, void *ptr, void* endOfSearchArea);

//en funktion som kör dfs för att hitta alla objekt och behålla de som faktiskt är pekare

//en funktion som markerar 

#endif // __TRAVERSE_STACK_H__

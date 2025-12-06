
#include "traverse_stack.h"
#include <stdint.h>
#include <setjmp.h>
#include <stdlib.h>
#include "gc.h"
#include "header/bit_manip.h"
#include "header/footer.h"
#include <stdio.h>


#define SPACE_SIZE 2048
extern char **environ;




// Save registers to the stack to avoid missing potential pointers in registers
#define Dump_registers()                     \
    jmp_buf env;                             \
    setjmp(env);




//helper function
static bool pointer_check_vaild(uintptr_t pointer){
    return true;
}

void* find_header_address_through_footer(heap_t *h, void *ptr, void* endOfSearchArea) {
    while (ptr < endOfSearchArea) {
        //we read 8 bytes at a time
        const uint64_t current_value = *((uint64_t*)ptr);

        if (confirm_tag(current_value)) {
            //bytes that the object was
            
            const uint8_t object_bytes = get_size_bits_field(current_value);

            //now if we jump back those bytes, we end up at start of ptr, and HEADER_SIZE more btyes
            //we end up at start of header
            //lets there
            const uint64_t bytes_to_jump_back = object_bytes + HEADER_SIZE_BYTES;
            void* header_start_ptr = (char*)ptr - bytes_to_jump_back;
            return header_start_ptr;
        } 

        // move forward by 8 bytes for the next iteration
        ptr = (char*)ptr + 8;
    }

    // we did not find a footer
    return NULL;
}



bool validate_ptr(heap_t *h, void* ptr) {
    unsigned char* start = (unsigned char *)h->spaces[0];
    unsigned char* heap_end = (unsigned char *)h->spaces[h->number_of_spaces - 1] + SPACE_SIZE;

    if (ptr < (void *)start || ptr > (void *)heap_end) {
        return false;
    }

    size_t bit =  ((unsigned char*)ptr - (unsigned char*)start)/16; //bit in allocmap
    size_t bit_index = bit % 64;                                    // bit position inside the current word
    size_t map_index = bit / 64;
    bool ptr_valid = get_bit_at_index_from_msb(h->alloc_map[map_index], bit_index) > 0;  

    return ptr_valid;
}


/**
 * Traverse the stack and collect all potential root pointers that point into the heap.
 * These pointers are stored in a dynamically allocated array, which is returned to the caller.
 * The size of the array is stored in the variable pointed to by `count`.
 *
 * @param heap_start The start address of the heap.
 * @param heap_end The end address of the heap.
 * @param count Pointer to a variable where the number of found pointers will be stored.
 * @return A dynamically allocated array containing all potential root pointers.
 */
void **root_ptrs(heap_t *h, size_t *count) {
    Dump_registers();

<<<<<<< HEAD
    void *heap_start = h->spaces[0];
    uintptr_t heap_start_addr = (uintptr_t)heap_start;
    uintptr_t heap_end_addr = (uintptr_t)((unsigned char*)h->spaces[h->number_of_spaces-1] + SPACE_SIZE);
   
=======
>>>>>>> footer
    //top and bottom
    uintptr_t stack_top = (uintptr_t)__builtin_frame_address(0);//top
    uintptr_t stack_bottom = (uintptr_t)environ;//bottom


    // Handle stack growth direction
    if (stack_top < stack_bottom) {
        uintptr_t temp = stack_top;
        stack_top = stack_bottom;
        stack_bottom = temp;
    }

    size_t capacity = 128;
    size_t found = 0;
    void **pointers = malloc(capacity * sizeof(void *));


    for (uintptr_t *current = (uintptr_t *)stack_top; current > (uintptr_t *)stack_bottom; --current) {
<<<<<<< HEAD
    
=======


>>>>>>> footer
        if (validate_ptr(h, (void *)*current)) {


            if(h->unsafe_stack){
                if(pointer_check_vaild((uint64_t)*current)){
                    // do nothing
                } else {
                    continue;
                }
            }
            if (found >= capacity) {
                capacity *= 2;
                void **new_pointers = realloc(pointers, capacity * sizeof(void *));
                pointers = new_pointers;
            }
            printf("\n");
            printf("Current :%p | points to%p | at index: %d\n", current, *(void **)current, (int)found);
            pointers[found++] = (void *)current;
        }
    }

    
    *count = found;
    return pointers;
}




//loop througth the stack and change every pointer it find to dbg_value
void invalidate_ptrs_at_heap(heap_t *h, void *dbg_value) {
    
    size_t count = 0; 
    void** pointers = root_ptrs(h, &count); 

    for(int i = 0; i < count; i++) {
        *(void **)pointers[i] = dbg_value;
    }
}


// int main() {
//     size_t count;
//     heap_t *heap = h_init(1024 * 1024, true, 0.75f);


//     // Example pointer into the heap
//     void *ptr = heap->spaces[0];
//     printf("Pointer %p\n", ptr);


//     void *ptr2 = heap->spaces[1];
//     printf("Pointer %p\n", ptr2);
//     ptr2 = NULL;
//     void *big_number_as_pointer = (void*)0x7fa8cadea410;
//     uintptr_t big_number = 0x7fa8cadea410;


//     invalidate_ptrs_at_heap(heap, NULL);


//     // Call root_ptrs
//     void **roots = root_ptrs(heap, &count);
//     //the root_ptrs here find every pointer in the stack, although the


//     // Print the results
//     printf("Found %zu root pointers:\n", count);
//     for (size_t i = 0; i < count; i++) {
//         printf("Pointer %zu: %p\n", i + 1, roots[i]);
//     }
// }
// it works great. really unsure waht they mean by invaild pointer.

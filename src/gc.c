#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <assert.h>

#include "common.h"
#include "traverse_stack.h"
#include "header/format_str.h"
#include "gc.h"
#include "header/format_str.h"
#include "traverse_stack.h"
#include "header/bit_vector.h"
#include "header/bit_manip.h"
#include "header/raw_layout.h"
#include "header/header.h"
#include "header/footer.h"
#include "utils.h"

#include "header/header.h"


#define SPACE_SIZE 2048

heap_t *h_init(size_t bytes, bool unsafe_stack, float gc_threshold)
{
    // return null if bytes not dividable by 2048*2, we need half active half deactivated
    if (bytes % (SPACE_SIZE*2) != 0)
    // return null if bytes not dividable by 2048*2, we need half active half deactivated
    if (bytes % (SPACE_SIZE*2) != 0)
    {
        return NULL;
    }
    // bytes should be more than 0
    if (bytes <= 0)
    {
        return NULL;
    }
    // treshhold must be 0.0 < i <= 1.0
    if (gc_threshold > 1 || gc_threshold <= 0)
    {
        return NULL;
    }

    // lets require a heap of atleast 2048*4 bytes
    if (bytes < 2048 * 4)
    {
        return NULL;
    }

    // number of sides = bytes / 2048
    int number_of_spaces = bytes / SPACE_SIZE;
    // allocate space for struct
    heap_t *heap = calloc(1, sizeof(heap_t));
    // allocates pointers to spaces
    heap->spaces = calloc(number_of_spaces, sizeof(void *));

    heap->size = bytes;
    heap->space_size = SPACE_SIZE;
    heap->unsafe_stack = unsafe_stack;
    heap->gc_threshold = gc_threshold;
    heap->number_of_spaces = number_of_spaces;
    heap->used_bytes = 0;
    heap->space_is_active = calloc(number_of_spaces, sizeof(bool));
    
    heap->alloc_map = calloc(bytes / (16 * 8), 1);
    
    // Example, heap size is 8192
    // The heap is split into segemnts of 16 bytes (segments available for allocating) 
    // 8192 / 16 = 512 SEGMENTS
    // There's 512 segements on heap
    // Which requires 512 bits (64 bytes) to keep track of
    // Calloc allocates 64 bytes through calloc(bytes / (16 * 8), 1);
    // 8192 / (16 * 8) = 64 BYTES (= 512 BITS)

    heap->alloc_map = calloc(bytes / (16 * 8), 1);

    // Example, heap size is 8192
    // The heap is split into segemnts of 16 bytes (segments available for allocating)
    // 8192 / 16 = 512 SEGMENTS
    // There's 512 segements on heap
    // Which requires 512 bits (64 bytes) to keep track of
    // Calloc allocates 64 bytes through calloc(bytes / (16 * 8), 1);
    // 8192 / (16 * 8) = 64 BYTES (= 512 BITS)

    // TODO: later on this will not be decided like this, only to test allocation
    // lets set all except last 2 spaces to active
    for (int i = 0; i < (number_of_spaces/2); i++)
    for (int i = 0; i < (number_of_spaces/2); i++)
    {
        heap->space_is_active[i] = true;
    }

    // allocates heap space
    void *heap_start = calloc(1, bytes);

    for (int i = 0; i < number_of_spaces; i++)
    {
        // make each array slot point to a space
        heap->spaces[i] = (void *)((unsigned char *)heap_start + i * SPACE_SIZE);
    }
    heap->alloc_front = heap->spaces[0];

    return heap;
}

void h_delete(heap_t *h)
{
    // SPACES_ACTIVE
    free(h->space_is_active);
    h->space_is_active = NULL;

    // START
    free(h->start);
    h->start = NULL;

    // ALLOC_FRONT
    h->alloc_front = NULL;

    // SPACES
    for (int i = 0; i < h->number_of_spaces; i++)
    {
        if (h->spaces[i])
        {
            h->spaces[i] = NULL;
        }
    }
    free(h->spaces);
    h->spaces = NULL;

    // ALLOC MAP
    free(h->alloc_map);
    h->alloc_map = NULL;

    free(h);
}

void h_delete_dbg(heap_t *h, void *dbg_value)
{
    invalidate_ptrs_at_heap(h, dbg_value);
    h_delete(h);
}

void set_alloc_map(heap_t *h, void *start_address, void *end_address)
{
    unsigned char *heap_start_address = (unsigned char *)h->spaces[0];

    unsigned char *start = (unsigned char *)start_address;
    unsigned char *end = (unsigned char *)end_address;

    size_t start_index = (start - heap_start_address) / 16;
    size_t end_index = (end - heap_start_address) / 16;
    
    for (size_t i = start_index; i < end_index; i++)
    {
        size_t bit_index = i % 64;                                                                     // bit position inside the current word
        size_t map_index = i / 64;                                                                     // byte inside the map
        h->alloc_map[map_index] = set_bit_at_index_from_msb(h->alloc_map[map_index], bit_index, true); // set the bit using bitwise OR
    }
}

// TODO: as of now it allocates space for  layout,
// why? user should not have to keep layout string alive?
// layout already has set the least 2 LSB
void *
h_alloc_struct(heap_t *h, char *layout)
{
    if (!layout)
    {
        return NULL;
    }
    // Let's create the header block,
    // layout is format string?
    format_str_t *frmt_string = create_format_str(layout);
    
    if (!frmt_string)
    {
        return NULL;
    }

    const uint64_t size = format_str_bytes(frmt_string);
    
    //TODO: 
    //uint64_t aligned_size = (raw_size + 7) & ~(uint64_t)7;


    option_t result = to_bit_vector(frmt_string);
    destroy_format_str(frmt_string);
    // necesery for alloc_map
    unsigned char *start_addrs = h->alloc_front;

    if (!result.success)
    {
        return NULL;
    }
    // if we cant fit the struct in heap we have to decline
    if (h_avail(h) < size + HEADER_SIZE_BYTES)
    {
        return NULL;
    }
    // struct must fit in one space
    if (HEADER_SIZE_BYTES + size > SPACE_SIZE)
    {
        return NULL;
    }
    // if layout is NULL
    if (layout == NULL)
    {
        return NULL;
    }
    // if layout is nothing ""
    if (*layout == '\0')
    {
        return NULL;
    }

    // now we want to know if there is space left on current space, one space is 2048
    // first lets figure out what space we are on,
    int space_index = (h->alloc_front - (unsigned char *)h->spaces[0]) / SPACE_SIZE;

    bool space_is_active = h->space_is_active[space_index];

    uint16_t space_left = SPACE_SIZE - (h->alloc_front - (unsigned char *)h->spaces[space_index]);

    bool struct_fits = size + HEADER_SIZE_BYTES <= space_left;

    // if space active and we got space left, we start inserting data, else we go to the next non active space
    if (!space_is_active || !struct_fits)
    {
        // lets walk to next active page or until end of spacec then we return NULL
        while (!space_is_active)
        {
            space_index++;
            // check we dont move to far!
            if (space_index >= h->number_of_spaces)
            {
                return NULL; // WE WENT OUT OF BOUNDS,
            }
            space_is_active = h->space_is_active[space_index];
        }
        // we found a space, lets move to it
        h->alloc_front = h->spaces[space_index];
    }

    // check that object still fits, we could have moved to
    // new page in a bad way, should be OK
    space_left = SPACE_SIZE - (((unsigned char*)h->alloc_front + HEADER_SIZE_BYTES) - (unsigned char *)h->spaces[space_index]);
    struct_fits = size + HEADER_SIZE_BYTES <= space_left;
  
    if (!struct_fits)
    {
        return NULL;
    }

    //create the header from bitvector (Dont remove, causes segfault somehow)
    format_str_t *format_string = create_format_str(layout);
    option_t bit_vec_res = to_bit_vector(format_string);
    destroy_format_str(format_string);
    uint64_t bit_vec = result.bit_vector;

    // copy the header into the current position in the heap
    *(uint64_t *)h->alloc_front = bit_vec; // Copy 8 bytes (size of header)


    h->alloc_front += HEADER_SIZE_BYTES; // lets move alloc front forward the size of header
    // Pntr to start of data block
    void *data_ptr = h->alloc_front;

    h->alloc_front += size; // Move the allocation pointer forward by the structure size
    //now we want to push on the footer.

    // FOOTER
    uint8_t footer_size = size;
    uint64_t footer = create_footer(footer_size);
    *(uint64_t *)h->alloc_front = footer;
    //move forward footer size
    h->alloc_front += HEADER_SIZE_BYTES;
    unsigned char *end_addrs = h->alloc_front;
    set_alloc_map(h, start_addrs, end_addrs);
    h->used_bytes += size + HEADER_SIZE_BYTES*2;


    void *header_ptr = (unsigned char *)data_ptr - 1;
    
    // Bör bli något mellan 0-2032
    uint64_t header_ptr_offset_from_space_ptr = (uint64_t)header_ptr - (uint64_t)(h->spaces[space_index]);

    uint64_t bit_index = header_ptr_offset_from_space_ptr / MIN_OBJ_SIZE; // Ger index mellan 0-127

    // TODO: gör om, finns nog smartare lösning
    // int offset = 0;
    // if (data_ptr >= h->spaces[space_index] + SPACE_SIZE/2)
    // {
    //     offset = 1;
    // }

    // om index är 64 vill vi egentligen gå till nästa uint64
    // så ta index - 64 (64 - 64 = 0) och hoppa till nästa uint64 (offset = 1)
    int offset = 0;
    if (bit_index > 63)
    {
        bit_index -= 64;
        offset = 1;
    }
    
    // index of the uint64 we're looking for in alloc_map
    uint64_t map_index = space_index * 2 + offset;

    for (size_t i = 0; i < size + HEADER_SIZE_BYTES; i += MIN_OBJ_SIZE)
    {
        // Check if we cross the middle of the space
        // (one space require 2 uint64s, we may have to jump to the next uint64)
        if (bit_index > 63) 
        {
            map_index += 1; // jump to next uint64
            bit_index = 0; // reset bit_index to start at beginning of next uint64 
        }
        *(h->alloc_map + map_index) = set_bit_at_index_from_msb(*(h->alloc_map + map_index), bit_index, true);
        bit_index++;
    }
    h->used_bytes += size + HEADER_SIZE_BYTES;
    return data_ptr; // Return the pointer to the newly allocated object (header + data)
}


void *
h_alloc_raw(heap_t *h, const size_t bytes)
{
    if (h_avail(h) < bytes)
    {
        return NULL;
    }
    if (bytes <= 0) {
        return NULL;
    }

    unsigned char *start_addrs = h->alloc_front;

    // TODO: no check if it fits in a Space

    header_t *header = create_raw_header(bytes);

    memcpy(h->alloc_front, &header->data, HEADER_SIZE_BYTES);
    h->alloc_front += HEADER_SIZE_BYTES;
    
    header_destroy(header);

    void *new_object = h->alloc_front;
    h->alloc_front += bytes;

    //place footer
    uint8_t footer_size = bytes;
    uint64_t footer = create_footer(footer_size);
    *(uint64_t *)h->alloc_front = footer;
    //move forward
    h->alloc_front += HEADER_SIZE_BYTES;

    unsigned char *end_addrs = h->alloc_front;

    
    set_alloc_map(h, start_addrs, end_addrs);
    h->used_bytes += HEADER_SIZE_BYTES*2 + bytes;
    h->used_bytes += HEADER_SIZE_BYTES + bytes;

    return new_object;
}

//helper function, this function process the three cases
static void
garbage_collection(heap_t* h, void* root)
{
    if (root == NULL){
        puts("root is null");
        return;
    }
    if (*(void**)root < (void*)0xFFF) {
        puts("root is to small");
        return;
    }

    puts("");
    //printf("current root %p", root);

    void* header = find_header_address_through_footer(h, *(void**)root,  *(void**)root + SPACE_SIZE);
    
    if (header == NULL){
        puts("header is null");
        return;
    }

    // == från och med här har vi en giltig header ==
    uint64_t bytes_offset = *(void**)root - header;
    
    header_t* header_obj = header_new(*(uint64_t*)header);
    HEADER_TYPE_T header_type = get_header_type(header_obj);

    // Steg 1, identifiera vilken typ av header
    if (header_type == BIT_VECTOR) {
        puts("Header was bit vector");
        // If we came here, first time reallocating this structure

        //1. reallocate this structure on the heap that has now been flipped
            // 1.1   We need a format string, how great that we have a function
            format_str_t* frmt_str =  bit_vector_to_format_string(*(uint64_t*)header);
            assert(frmt_str);
            assert(frmt_str->str);
            // 1.2 call allocate with heap!
            void *new_obj = h_alloc_struct(h, frmt_str->str);
            assert(new_obj);
            void *new_header = ((unsigned char*)new_obj - 8);
            
            assert(new_obj - new_header == 8);
            assert(new_obj != *(void**)root);
            assert(new_obj != NULL);
            assert(new_header != NULL);
    
            //1.3 we have a new home, lets move in the furniture
                //1.3.1 how big is our home
                uint64_t size = get_size_bits_field(*(uint64_t*)header);
                //assert no bigger than uint8_t, 1 byte
                assert(size <= 0xFF); //max 1 byte can hold
                
            memcpy(new_obj, *(void**)root, size);
            
            //1.4 Clean up for future root_ptrs pointing to the old header
            
            header_t* forward_address_header = header_new(*(uint64_t*)header);
            void* header_address = new_header; //FIXME either new_header or &new_header
            
            set_header_forward_address(forward_address_header, header_address);
            *(uint64_t*)header = forward_address_header->data;

            //1.5 change root now
            *(void**)root = header_address + bytes_offset;
            
        

        //TODO: here set header forwardaddress to new header
        //get header address -> points to the new header
        // walk byte_offset bytes and repoint rooter to here!


        return;
    } else if (header_type == FORWARDING_ADDRESS) {
        puts("header is forward adresss");
        // we know how far we point!
        void* new_header_address = (void*)set_bit_at_index_from_lsb(*(uint64_t*)header, 0, false);
        new_header_address = (void*)set_bit_at_index_from_lsb((uint64_t)new_header_address, 0, false);
 
        *(void**)root = new_header_address + bytes_offset;
        
        return;
    uint64_t item_header_bits = *(uint64_t*)((char*)(*(void**)root) - 8); // try to get the header of the object root point to
    header_t *item_header = header_new(item_header_bits);
    HEADER_TYPE_T header_type = get_header_type(item_header); // get header type
    
    if (header_type == BIT_VECTOR || header_type == PTR_TO_FORMAT_STRING) { // if the header is a bit vektor
        size_t size = header_get_size(item_header); // the size byte(the first two btes)
        bit_vector_t layout = get_header_layout(item_header);
        size_t distance_to_pointer = 0; // a counter to calculate where to find pointers

        for (int i = 0; i < 46 ; ++i) { //gå igenom all bit i layout byte
            uint64_t bit = (layout >> i) & 1;  // får current bit
            if (bit == 0){
                distance_to_pointer += 4; // om det är 0 så är det inte ett pointer, men counter ska plus med 4 byte
            } else {
                void *sub_pointer = (void *)((char *)(*(void**)root) + distance_to_pointer); // hiitar den pointer
                garbage_collection(h, sub_pointer); // recursive call funktionen för att kolla om det finns pekare i den undre pekare
                distance_to_pointer += 8; // och det blir +8 byte för pekare
            }
        }
        char * for_st = bit_vector_to_format_string(item_header->data)->str;
        void* new_allocated = h_alloc_struct(h, for_st); // allokera platsen på den nya space
        memcpy(new_allocated, root, size); // copy the data

        set_header_forward_address(item_header, new_allocated); // en påhittad funktion som tar in header och gör det om till forwardinf address        
        
        *(uint64_t*)((char*)(*(void**)root) - 8) = item_header->data;
        
        //set at root 
        *(void **)root = new_allocated; // ändra root så att det pekar på den ny addressen (den här ändra bara lokal kopia)
       
    } else if (header_type == RAW_LAYOUT){
        puts("header was RAW");
        //RAW
        return; 
    }
    puts("header was NOTHING????");
    
    return;
}


        //raw
        size_t size = header_get_size(item_header); 
        void* new_allocated = h_alloc_raw(h, size);
        memcpy(new_allocated, root, size); // copy  data
        set_header_forward_address(item_header, new_allocated);
        *(uint64_t*)((char*)(*(void**)root) - 8) = item_header->data;
        *(void **)root = new_allocated; // ändra root så att det pekar på den ny addressen
    }  else { 
        //do nothing
        //det här är forwarding address vilken betyder att object har redan blev flyttad
    }
}

size_t
h_gc(heap_t *h)
{
   return h_gc_dbg(h, false);
}

size_t
h_gc_dbg(heap_t *h, bool unsafe_stack)
{  
    size_t num_roots = 0;
    void **root_pointers;

    // === Fetch root pointers ===

    if (unsafe_stack){
        root_pointers = root_ptrs(h, &num_roots); // hittar alla root pointer där det är osäkert om alla pekare är säkert eller inte?
    } else {
        root_pointers = root_ptrs(h, &num_roots); // hitta alla root pekare där man är säkert på att pekare är giltig.
    }

    // Keep for future reference

    // === Flip Spaces ===
 
    size_t old_used_mermory = h_used(h); // hur mycket mermory som används innan garabage collction
    for(size_t i = 0; i < h->number_of_spaces; i++) {
       h->space_is_active[i] = !(h->space_is_active[i]);
    }

    // === Set bump-pointer ===
    
    for(size_t i = 0; i < h->number_of_spaces; i++) { // omvandla alla active space till icje active och så vidare
       h->space_is_active[i] = !(h->space_is_active[i]);
    } // data i header omvandlade för någon andlenignÅ? !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    for (int i = 0;  i < h->number_of_spaces; i++) { // hitta den första activa space och sätter bump-pointer till starten av space
        if(h->space_is_active[i]) {
            h->alloc_front = h->spaces[i];
            break;
        }
    }

  

    h->used_bytes = 0; // reset antal byte som används

    // === Call garbage collection on each! ===
    if (!(num_roots <= 0)) {
        for(size_t i = 0; i < num_roots - 1; i++) { // garbage collctin på alla root pointer
            void *root_pointer = root_pointers[i];

            // //TODO: Remove these checks, now we look for footer
            // if((uint64_t)(*(void**)root_pointer) <= 0x4){
            //     continue;
            // }

            // if( ((uint64_t)h->spaces[0] - (uint64_t)(*(void**)root_pointer)) % 4 != 0 ) { 
            //     continue;
            // }

            garbage_collection(h, root_pointer);
        }
    }

    //TODO: set alloc map to false for all spaces that was deactivated

    const size_t new_used_memory = h_used(h); // kollar hur mycket mermory som man använder nu
    size_t new_used_memory = h_used(h); // kollar hur mycket mermory som man använder nu
    printf("old: %zu,  new: %zu ", old_used_mermory, new_used_memory);

    return old_used_mermory - new_used_memory; // returna antakl byte som togs bort.
}

size_t 
h_avail(const heap_t *h)
{
    if (h->size < h->used_bytes)
    {
        return 0;
    }
    return h->size - h_used(h);
}

size_t 
h_used(const heap_t *h)
{
    return h->used_bytes;
    return h->used_bytes;
}

//
// Created by Viktor Liljenberg on 2024-12-17.
//
#include "header.h"
#include "bit_manip.h"

header_t *header_new(const u_int64_t data) {
    header_t *header = malloc(sizeof(header_t));
    header->data = data;
    return header;
}

void header_destroy(header_t *header) {
    free(header);
}

size_t ptr_to_format_string_get_size(const header_t *header) {
    char *string = (char*)header->data;
    //make a format string out of these chars
    format_str_t* frmt_str = create_format_str(string);
    const size_t bytes = format_str_bytes(frmt_str);
    return bytes;
}
//TODO: what happens with forwarding_addrs
size_t forwarding_address_get_size(const header_t *header) {
    //the next header should be next to it
    //FIXME
    return 0;
}
size_t raw_layout_get_size(const header_t *header) {
    //shift header->data right twice to get the size
    const uint64_t size = header->data >> 2;
    return size;
}
size_t bit_vector_get_size(const header_t *header) {
    const uint64_t size = get_size_bits_field(header->data);
    return size;
}

size_t header_get_size(const header_t *header) {
    const enum HEADER_TYPE h_type = get_header_type(header);
    switch (h_type) {
        case PTR_TO_FORMAT_STRING:
            return ptr_to_format_string_get_size(header);
        case FORWARDING_ADDRESS:
            return forwarding_address_get_size(header);
        case RAW_LAYOUT:
            return raw_layout_get_size(header);
        case BIT_VECTOR:
            return bit_vector_get_size(header);
        default:
            return 0; //something went wrong
    }
}

uint64_t header_get_address(const header_t *header) {
    const HEADER_TYPE_T h_type = get_header_type(header);
    if (h_type == PTR_TO_FORMAT_STRING || h_type == FORWARDING_ADDRESS) {
        const uint64_t masked_data = mask_from_lsb(header->data, 2);
        return masked_data;
    }
    return 0;
}

HEADER_TYPE_T get_header_type(const header_t *header) {
    const uint64_t masked_data = mask_from_msb(header->data, 62);
    switch (masked_data) {
        case 0: return PTR_TO_FORMAT_STRING;
        case 1: return FORWARDING_ADDRESS;
        case 2: return RAW_LAYOUT;
        case 3: return BIT_VECTOR;
        //to satisfy compiler, we know that 2 bits can only hold 0-3
        default: return PTR_TO_FORMAT_STRING;
    }
}

uint64_t get_header_layout(const header_t *header) {
    const HEADER_TYPE_T h_type = get_header_type(header);
    if (h_type == PTR_TO_FORMAT_STRING) {
        char *string = (char*)header->data;
        format_str_t* frmt_str = create_format_str(string);
        const bit_vector_t bit_vec =  to_bit_vector(frmt_str).bit_vector;
        return get_layout_bits_field(bit_vec);
    }
    if (h_type == BIT_VECTOR) {
        return get_layout_bits_field(header->data);
    }
    return 0;
}

void set_header(header_t *header, u_int64_t data) {
    header->data = data;
}

uint64_t get_header(header_t *header){
    return header->data;
}


void set_header_forward_address(header_t *header, void* address){
    uint64_t forward_address = (uint64_t) address;
    forward_address = forward_address & ~0b11;
    forward_address = forward_address | 0b01;
    set_header(header, forward_address);
}



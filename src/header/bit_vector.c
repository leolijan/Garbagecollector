//
// Created by Viktor Liljenberg on 2024-12-19.
//

#include "bit_vector.h"

#include "bit_manip.h"

bool enough_bits_for_layout(format_str_t* format){
	size_t required_bits = 0;
    char* str = format->str;
    while(*str != '\0'){
      switch ( *str ) {
            case '*': required_bits += POINTER_INTERPRETED_SIZE; break;
            case 'c': required_bits += CHAR_INTERPRETED_SIZE; break;
            case 'i': required_bits += INTEGER_INTERPRETED_SIZE; break;
            case 'l': required_bits += LONG_INTERPRETED_SIZE; break;
            case 'f': required_bits += FLOAT_INTERPRETED_SIZE; break;
            case 'd': required_bits += DOUBLE_INTERPRETED_SIZE; break;
            default: break;
      }
      str++;
    }
    return required_bits <= MAX_LAYOUT_BITS;
}

uint64_t get_layout_bits(const format_str_t* format) {
    //thread on from msb to lsb
    uint64_t layout_bits = 0;
    int iter = 0;
    char* str = format->str;
    while (*str != '\0') {
        switch ( *str ) {
            case '*': {
                layout_bits = set_bit_at_index_from_msb(layout_bits, iter, true);
                iter += 1;
                break;
            }
            case 'c': {
                layout_bits = set_bit_at_index_from_msb(layout_bits, iter, false);
                iter += CHAR_INTERPRETED_SIZE;
                break;
            }
            case 'i': {
                layout_bits = set_bit_at_index_from_msb(layout_bits, iter, false);
                iter += INTEGER_INTERPRETED_SIZE;
                break;
            }
            case 'l': {
                layout_bits = set_bit_at_index_from_msb(layout_bits, iter, false);
                iter += LONG_INTERPRETED_SIZE;
                break;
            }
            case 'f': {
                layout_bits = set_bit_at_index_from_msb(layout_bits, iter, false);
                iter += FLOAT_INTERPRETED_SIZE;
                break;
            }
            case 'd': {
                layout_bits = set_bit_at_index_from_msb(layout_bits, iter, false);
                iter += DOUBLE_INTERPRETED_SIZE;
                break;
            }
            default: {
                return 0x0;
            }
        }
        str++;
    }
    return layout_bits;
}

option_t to_bit_vector(format_str_t* format) {
    // Check that input is small enough to be held
    if (!enough_bits_for_layout(format)) {
        option_t result = { .success = false };
        return result;
    }


    // get size
    uint64_t size = format_str_bytes(format);
    size = size << 56; //place in most significant byte
    // get layout and shift 8 right
    uint64_t layout_bits = get_layout_bits(format);
    layout_bits = layout_bits >> 8;
    uint64_t merged_bits = layout_bits | size;
    // set type
    merged_bits = set_bit_at_index_from_lsb(merged_bits, 0, true);
    merged_bits = set_bit_at_index_from_lsb(merged_bits, 1, true);

    option_t result = { .success = true, .bit_vector = merged_bits };
    return result;
}

uint64_t get_size_bits_field(bit_vector_t bits) {
    bits = bits >> 56;
    return bits;
}

uint64_t get_layout_bits_field(bit_vector_t bits) {
    bits = mask_from_lsb(bits, 2);
    bits = bits << 8;
    return bits;
}

format_str_t* bit_vector_to_format_string(bit_vector_t bits) {

    size_t size = get_size_bits_field(bits);
    uint64_t layout_bits = get_layout_bits_field(bits);
    //now we create a buffer string to push characters to,
    //we need atmost 1 char every 4 bytes, (size is bytes)
    uint32_t buffer_size = size / 4;
    char* buffer_str = calloc(buffer_size + 1, sizeof(char));
    uint64_t current_accumulated_size = 0;
    for (size_t i = 0; i < 54; i++) {
        //since layout bits are 54 long we want to iterate through until we reach size
        bool current_bit = get_bit_at_index_from_msb(layout_bits, i);
        if (current_bit) {
            current_accumulated_size += POINTER_INTERPRETED_SIZE * 4;
            //lets add '*' to the format string
            buffer_str[i] = '*';
        } else {
            current_accumulated_size += CHAR_INTERPRETED_SIZE * 4;
            //lets add 'i' to the format string
            buffer_str[i] = 'i';
        }

        if (current_accumulated_size >= size) {
            break;
        }

    }
    format_str_t* frmt_str = create_format_str(buffer_str);
    return frmt_str;
}

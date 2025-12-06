
//
// Created by Viktor Liljenberg on 2024-12-19.
//
#ifndef BIT_VECTOR_H
#define BIT_VECTOR_H

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include "format_str.h"

/**
* | 8 bits (Size)| 54 bits (Layout)| 2 bits (Type) |
*
* size is from lsb to msb like a normal number is read
* layout is from msb to lsb since it is not supposed to be interpreted as a number
* Type lsb to msb
 */
typedef u_int64_t bit_vector_t;

/**
* Takes in string and returns a bitvector
*   Pre: enough_bits_for_layout(input) must be true
*   Parameters:  input, from fromat_str/interpreted_format_str(char* str)
*   Return: Option_t with bit_vector
*/
option_t to_bit_vector(format_str_t* format);

/**
*  Return if Layout has enough bits to represent input
*  Parameters:  input, from fromat_str/interpreted_format_str(char* str)
*/
bool enough_bits_for_layout(format_str_t *format);



uint64_t get_size_bits_field(bit_vector_t bits);
//read first 54 bits
uint64_t get_layout_bits_field(bit_vector_t bits);


/***
 *  returns a format str with same ptr layout and byte size.
 */
format_str_t* bit_vector_to_format_string(bit_vector_t bits);




//set_body
//set_size
//get_body
//get_size

#endif //BIT_VECTOR_H

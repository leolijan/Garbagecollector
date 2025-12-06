//set_from_lsb
//get_layout_type
//set_layout_type
//reset_layout_type

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


/**
* Converts uint64_t to its bit representation as following string
* "00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000"  8bytes
*/
char* bits_to_bitstring(uint64_t bits);

/**
*  Converts a string representation of bits,
*  Takes all formats with 64 bits, no mather \n and spaces
*
*  EX 1:
* "00000000 00000000 00000000 00000000   4bytes
*  00000000 00000000 00000000 00101010"  4bytes
*
*  EX 2:
*  "00000000 00000000 00000000 00000000 00000000 00000000 00000000 00101010"  8bytes
*
*  EX 3:
*  "0000000000000000000000000000000000000000000000000000000000101010"  8bytes
*/
uint64_t bitstring_to_bits(char* bitstring);


/**
* Reverses the bits
* EX: 1111 0000 ... 0000 => 0000 ... 0000 1111
*/
uint64_t reverse_bits(uint64_t source);

/**
* Set The Most Significant Bits of dest
*/
uint64_t set_upper_4bytes(uint64_t dest, uint32_t bits);

/**
* Set the Least Significant Bits of dest
*/
uint64_t set_lower_4bytes(uint64_t dest, uint32_t bits);

/**
* Copy x bits from source -> dest, starting from lsb
* EX:  dest:   0000 ... 1100 1010 1000
*      source: 0000 ... 0011 1111 1111
*      number_of_bits = 10
*      =>      0000 ... 1111 1111 1111
*/
uint64_t copy_bits_from_lsb(uint64_t dest, uint64_t source, uint64_t number_of_bits);

/**
* Copy x bits from source -> dest, starting from msb
* EX:  dest:   0000 1100 1010 ... 0000
*      source: 1111 0100 0000 ... 0000
*      number_of_bits = 6
*      =>      1111 0100 1010 ... 0000
*/
uint64_t copy_bits_from_msb(uint64_t dest, uint64_t source, uint64_t number_of_bits);

// EX: number = 1110 0000 ... 0111   |  mask_from_lsb(number, 1) => 1110 0000 ... 0110
// EX: number = 1110 0000 ... 0111   |  mask_from_lsb(number, 2) => 1110 0000 ... 0100
// EX: number = 1110 0000 ... 0111   |  mask_from_lsb(number, 0) => 1110 0000 ... 0111
uint64_t mask_from_lsb(uint64_t source, uint64_t bits_to_reset);

// EX: number = 1110 0000 ... 0111   |  mask_from_lsb(number, 1) => 0110 0000 ... 0111
// EX: number = 1110 0000 ... 0111   |  mask_from_lsb(number, 2) => 0010 0000 ... 0111
// EX: number = 1110 0000 ... 0111   |  mask_from_lsb(number, 0) => 1110 0000 ... 0111
uint64_t mask_from_msb(uint64_t source, uint64_t bits_to_reset);

/**
* Sets bit at index from lsb
* EX: number = 1110 0000 ... 0111  | set_bit_at_index_from_lsb(number, 0, 0)  => 1110 0000 ... 0110
* EX: number = 1110 0000 ... 0111  | set_bit_at_index_from_lsb(number, 1, 0)  => 1110 0000 ... 0101
* EX: number = 1110 0000 ... 0111  | set_bit_at_index_from_lsb(number, 2, 0)  => 1110 0000 ... 0011
*/
uint64_t set_bit_at_index_from_lsb(uint64_t source, uint64_t index, bool value);

/**
* Sets bit at index from msb
* EX: number = 1110 0000 ... 0111  | set_bit_at_index_from_msb(number, 0, 0)  => 0110 0000 ... 0111
* EX: number = 1110 0000 ... 0111  | set_bit_at_index_from_msb(number, 1, 0)  => 1010 0000 ... 0111
* EX: number = 1110 0000 ... 0111  | set_bit_at_index_from_msb(number, 2, 0)  => 1100 0000 ... 0111
*/
uint64_t set_bit_at_index_from_msb(uint64_t source, uint64_t index, bool value);

/**
* Gets bit at index from msb
*/
uint64_t get_bit_at_index_from_msb(uint64_t source, uint64_t index);



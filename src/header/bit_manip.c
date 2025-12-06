
#include "bit_manip.h"
#include "../utils.h"

char* 
bits_to_bitstring(uint64_t bits) 
{
  // we need to allocate mem
  char* bitstring = (char*) malloc(72 * sizeof(char));  /* 8*8 + 7 spaces + \0  */
  if (bitstring == NULL) 
    return NULL; // failed allocation
  int bit_index = 0;
  // iterate through each bit with an index from msb from lsb
  for (int i = 63; i >= 0; i--) 
    {
    //masks to read of the nth bit of bitstring
     bitstring[bit_index++] = (bits & (1ULL << i)) ? '1' : '0';
    //we insert space if eight't index
     if (i % 8 == 0 && i != 0) 
       bitstring[bit_index++] = ' ';
    }
  // Insert null terminate the string
  bitstring[bit_index] = '\0';

  return bitstring;
}


uint64_t bitstring_to_bits(char* bitstring) {
  char* cleared_str = remove_all_spaces(bitstring);
  // Empty bits
  uint64_t bits = 0;

  for (int i = 0; i < 64; i++) {
    bits = (bits << 1) | (cleared_str[i] == '1');
  }
  return bits;
}


/**
* Using an bitwise reverse algorithm in O(1)
*/
uint64_t reverse_bits(uint64_t source) {
  //masks every other and shift right, masks every other and shift left then OR to merge
  source = ((source >> 1) & 0x5555555555555555) | ((source & 0x5555555555555555) << 1);
  // Selects every 2 bits, and swap in same way
  source = ((source >> 2) & 0x3333333333333333) | ((source & 0x3333333333333333) << 2);
  // every 4 bits, swapping them
  source = ((source >> 4) & 0x0F0F0F0F0F0F0F0F) | ((source & 0x0F0F0F0F0F0F0F0F) << 4);
  source = ((source >> 8) & 0x00FF00FF00FF00FF) | ((source & 0x00FF00FF00FF00FF) << 8);
  source = ((source >> 16) & 0x0000FFFF0000FFFF) | ((source & 0x0000FFFF0000FFFF) << 16);
  source = ((source >> 32)) | (source << 32);
  //all bits will now be reversed
  return source;
}

uint64_t set_upper_4bytes(uint64_t dest, uint32_t bits){
  uint64_t masked_dest = dest & 0x00000000FFFFFFFF;
  uint64_t masked_bits = (uint64_t)bits << 32;
  uint64_t result = masked_dest | masked_bits;
  return result;
}

uint64_t set_lower_4bytes(uint64_t dest, uint32_t bits){
  uint64_t masked_dest = dest & 0xFFFFFFFF00000000;
  uint64_t masked_bits = (uint64_t)bits;
  uint64_t result = masked_dest | masked_bits;
  return result;
}

uint64_t copy_bits_from_lsb(uint64_t dest, uint64_t source, uint64_t number_of_bits){
  // we create a mask by shifting 1ULL to the left number_of_bits and then invert
  /*
      i.ex  dest: FFFF FFFF FFFF FFFF   source: FFFF FFFF FFFF DDDD and we copy 16bits from source

      ** Create mask for source **
      First we ened to addres an edge case, shifting 64 is undefined,
      hence we need to set the mask differently in that case, with that
      out of the way, lets go though the implementation

      first we create a mask to mask out source,
      0000 0000 0000 FFFF, now AND will only keep the 16 lowest bits
      => 0000 0000 0000 DDDD

     by inverting the mask we get it to maskm dest in similar way
      FFFF FFFF FFFF 0000, AND
      => FFFF FFFF FFFF 0000 (looks the same since dest is all F)

     Now we OR to merge them into
     => FFFF FFFF FFFF DDDD

  */
  uint64_t mask;
  if (number_of_bits == 64) {
    mask = 0xFFFFFFFFFFFFFFFF;
  } else {
    mask = (1ULL << number_of_bits) - 1;
  }
  uint64_t masked_source = source & mask;
  uint64_t masked_dest = dest & (~mask);
  uint64_t result = masked_dest | masked_source;

  return result;
}


uint64_t copy_bits_from_msb(uint64_t dest, uint64_t source, uint64_t number_of_bits) {
  // Look at documentation for copy from LSB , same logic just  flipped around
  uint64_t mask;

  if (number_of_bits == 64) {
    mask = 0xFFFFFFFFFFFFFFFF;
  } else {
    mask = ((1ULL << number_of_bits) - 1) << (64 - number_of_bits); // ones for number_of_bits times then 0
    // i.ex numebr_of_bits = 16, mask = FFFF 0000 0000 0000
  }

  uint64_t masked_source = source & mask;
  uint64_t masked_dest = dest & (~mask);
  uint64_t result = masked_dest | masked_source;

  return result;
}

uint64_t mask_from_lsb(uint64_t source, uint64_t bits_to_reset) {
  if (bits_to_reset == 0) {
    return source;
  }
  if (bits_to_reset >= 64) {
    return 0x0;
  }
  uint64_t mask = ~((1ULL << bits_to_reset) - 1);
  return source & mask;
}

uint64_t mask_from_msb(uint64_t source, uint64_t bits_to_reset) {
  if (bits_to_reset == 0) {
    return source;
  }
  if (bits_to_reset >= 64) {
    return 0x0;
  }
  uint64_t mask = ~(((1ULL << bits_to_reset) - 1) << (64 - bits_to_reset)); // Preserve LSBs
  return source & mask;
}

uint64_t set_bit_at_index_from_lsb(uint64_t source, uint64_t index, bool value) {
  uint64_t mask = 1ULL << index;

  if (value) {
    source |= mask;
  } else {
    source &= ~mask;
  }
  return source;
}

uint64_t set_bit_at_index_from_msb(uint64_t source, uint64_t index, bool value) {
  uint64_t lsb_index = 63 - index;

  uint64_t mask = 1ULL << lsb_index;
  if (value) {
    source |= mask;
  } else {
    source &= ~mask;
  }
  return source;
}

uint64_t get_bit_at_index_from_msb(uint64_t source, uint64_t index) {
  uint64_t mask = 0; 
  mask = set_bit_at_index_from_msb(mask, index, true);  
  uint64_t result = mask & source;                 
  return result; 
}



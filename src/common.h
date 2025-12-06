//TODO: lägg till dokumentation

#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>
#include <stddef.h>


typedef enum HEADER_TYPE {
    PTR_TO_FORMAT_STRING,
    FORWARDING_ADDRESS,
    RAW_LAYOUT,
    BIT_VECTOR
} HEADER_TYPE_T;

#define MAX_LAYOUT_BITS 54
#define MAX_STRUCT_ALLOCATION_SIZE_BYTES 56*4
#define MIN_OBJ_SIZE 16
#define HEADER_SIZE_BYTES 8
#define HEADER_LAYOUT_SIZE_BYTES 7
#define HEADER_INTERPRETED_SIZE 1

#define POINTER_INTERPRETED_SIZE 2
#define INTEGER_INTERPRETED_SIZE 1
#define DOUBLE_INTERPRETED_SIZE 2
#define FLOAT_INTERPRETED_SIZE 1
#define LONG_INTERPRETED_SIZE 2
#define CHAR_INTERPRETED_SIZE 1

//For format_str functions
typedef struct option {
    bool success;
    size_t total_bits;
    u_int64_t bit_vector;
} option_t;


#endif /* COMMON_H */



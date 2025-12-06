//
// Created by Viktor Liljenberg on 2024-12-17.
//


//You are welcome to move this file, or integrate with gc file, just temporary

#ifndef HEADER_H
#define HEADER_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "bit_vector.h"

typedef struct header header_t;

struct header {
    //the layout of this block is determined by the last 2 bits
    uint64_t data;
};

/**
 *
 * @param data
 * @return allocated header_t with data initialized to data
 */
header_t *header_new(u_int64_t data);

/**
 *
 * @param header
 * @brief deallocates header and its fields
 */
void header_destroy(header_t *header);


/**
 *
 * @param header
 * @return size_t, size(bytes) of upcoming block or 0 if type don't got size
 * @brief
 *    type PTR_TO_FORMAT_STRING => 0
 *    type FORWARDING_ADDRESS => size of corresponding bit vec
 *    type RAW_LAYOUT => 62 most significant bits
 *    type BIT_VECTOR => 1st most significant byte
 */
size_t header_get_size(const header_t *header);

/**
 *
 * @param header
 * @return u_int64_t, formated for mem address (type is masked away)
 * returns 0 if type is wrong, check pre
 * @pre type is PTR_TO_FORMAT_STRING or FORWARDING_ADDRESS
 * @brief masks type from data and returns it if type was of valid type
 *
 */
uint64_t header_get_address(const header_t *header);


/**
 * 
 * @param header
 * @return HEADER_TYPE
 * @brief
 *      00 => PTR_TO_FORMAT_STRING,
 *      01 => FORWADING_ADDRESS,
 *      10 => RAW_LAYOUT,
 *      11 => BIT_VECTOR
 */
HEADER_TYPE_T get_header_type(const header_t *header);

/**
 *
 * @param header
 * @return format_str explaining mem structure of upcoming block or 0 if invalid type
 * @brief extracts the layout, or follows ptr and convert to bit_vec
 * @pre must be of type PTR_TO_FORMAT_STRING or BIT_VECTOR
 * @post read only the first 54 bits from msb
 */
bit_vector_t get_header_layout(const header_t *header);

/**
 * @param header
 * @param data, data to be set
 * @brief sets header's data field to desired bits
 */
void set_header(header_t *header, u_int64_t data);

/**
 * @param header
 * @return returns header's data field
 */
uint64_t get_header(header_t *header);


/**
 * @param header
 * @param address 
 */
void set_header_forward_address(header_t *header, void* address);

#endif //HEADER_H
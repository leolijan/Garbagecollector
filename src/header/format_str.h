#ifndef FORMAT_STR_H
#define FORMAT_STR_H
#include "../common.h"
#include <stddef.h>

//TODO:lägg till dokumentation

/**
 * Struct representing the layout of a struct in a heap
 */
typedef struct format_str format_str_t;
struct format_str {
    char *str; //user inserted string EX: "3i***"
};


/**
 * Creates and Allocates format string with a interpreted string
 * @param str a String
 * @return format_str
 */
format_str_t* create_format_str(char *str);

/**
 * Deallocates format_str and its fields
 * @post Deallocates format_str
 */
void destroy_format_str(format_str_t *format_str);

/**
 * Creates and allocate an interpreted string from str
 * @param str
 * @param char* string
 * @return formated string struct
 * @example
 *   input: "3i**" => "iii**"
 *   input: "100" => "ccccc...ccc"
 */
char* interpreted_format_str(char *str);

/**
 * Calculate number of bytes
 */
size_t format_str_bytes(format_str_t *str);

//set_body
//get_body



#endif // FORMAT_STR_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h> // För SIZE_MAX

#include "format_str.h"
#include "../utils.h"

struct format_str *create_format_str(char *str)
{
    if (str == NULL)
    {
        return NULL;
    }

    format_str_t *format_str = calloc(1, sizeof(format_str_t));
    assert(format_str);
    assert(str);
    // allocated by interpreted_format_str
    char *inter_str = interpreted_format_str(str);
    if (inter_str == NULL)
    {
        return NULL;
    }
    else
    {
        format_str->str = inter_str;
        return format_str;
    }
}

void destroy_format_str(format_str_t *format_str)
{
    assert(format_str);
    free(format_str->str);
    free(format_str);
}

char *interpreted_format_str(char *str)
{
    assert(str);
    // Lets trim the white space before and after
    // TODO: trim_whitespace returns new allocated
    // MEM LEAK HERE
    char *trimmed_str = trim_whitespace(str);
    if (!trimmed_str)
        return NULL;

    // This process is a multi staged one
    // first we calculate the size so we can allocate a string
    // big enough,then we fill it

    // *** Step one Calculate alloc space
    size_t expanded_length = 0;
    const char *ptr = trimmed_str;

    while (*ptr != '\0')
    {
        if (*ptr == '0')
        {
            return NULL;
        }

        if (!isdigit(*ptr) && !(*ptr == 'i' || *ptr == 'c' || *ptr == 'd' || *ptr == 'l' || *ptr == '*' || *ptr == 'f'))
        {
            return NULL;
        }

        if (isdigit(*ptr))
        {

            // Read the full number
            int repeat_count = 0;
            while (isdigit(*ptr))
            {
                repeat_count = repeat_count * 10 + (*ptr - '0'); // Accumulate the number
                ptr++;
            }
            if (*ptr != '\0')
            {
                expanded_length += repeat_count;
                ptr++; // consume i in "10i" since it only is 10 chars
            }
        }
        else
        {
            // if not number we add one i.e: "iii"
            expanded_length += 1;
            ptr++;
        }
    }

    //  Lets alloc space for the str
    char *expanded_str = (char *)malloc(expanded_length + 1);
    if (!expanded_str)
    {
        return NULL;
    }

    char *write_ptr = expanded_str;
    ptr = trimmed_str;
    while (*ptr != '\0')
    {
        // if we copme across a number we want to read the whole numebr
        if (isdigit(*ptr))
        {
            // read the full number
            int repeat_count = 0;
            while (isdigit(*ptr))
            {
                repeat_count = repeat_count * 10 + (*ptr - '0');
                ptr++;
            }

            if (*ptr != '\0')
            {
                // we want "3x" => "" and not "xxx" hence we only add if valid postfix char
                if (*ptr == '*' || *ptr == 'c' || *ptr == 'i' || *ptr == 'l' || *ptr == 'f' || *ptr == 'd')
                {
                    for (int i = 0; i < repeat_count; i++)
                    {
                        *write_ptr++ = *ptr;
                    }
                    ptr++;
                }
                else
                {
                    ptr++;
                }
            }
            else
            {
                // This assumes c if no other char. i.ex "10" => "cccccccccc"
                for (int i = 0; i < repeat_count; i++)
                {
                    *write_ptr++ = 'c';
                }
            }
        }
        else
        {
            *write_ptr++ = *ptr++;
        }
    }

    // add NULL terminator
    *write_ptr = '\0';

    return expanded_str;
}

size_t format_str_bytes(format_str_t *str)
{
    assert(str);
    char *string = str->str;
    size_t bytes = 0;
    // lets walk through the string and add appropriate bytes for each symbol
    while (*string != '\0')
    {
        switch (*string)
        {
        case '*':
            bytes += POINTER_INTERPRETED_SIZE * 4;
            break;
        case 'c':
            bytes += CHAR_INTERPRETED_SIZE * 4;
            break;
        case 'i':
            bytes += INTEGER_INTERPRETED_SIZE * 4;
            break;
        case 'l':
            bytes += LONG_INTERPRETED_SIZE * 4;
            break;
        case 'f':
            bytes += FLOAT_INTERPRETED_SIZE * 4;
            break;
        case 'd':
            bytes += DOUBLE_INTERPRETED_SIZE * 4;
            break;
        default:
            break;
        }
        string++;
    }
    return bytes;
}

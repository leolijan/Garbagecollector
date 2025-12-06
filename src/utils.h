/*
 * File: utils.h
 * Purpose: A module with several manipulating utilities for the program.
 * Created by Viktor Liljenberg on 2024-12-19.
 */

#ifndef UTILS_H
#define UTILS_H /* Ensure this header is included only once */

/* A module with various utilitys for the program */

/* Standard library headers */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Function declarations for string manipulation utilities */

/**
 * Description...
 *
 * @param str
 * @return A new string...
 */
char *trim_whitespace (const char *str);

/**
 * Removes all spaces from a string
 *
 * @param str
 * @return A new string with all spaces removed or NULL.
 *         The string will be needed to be freed.
 */
char *remove_all_spaces (const char *str);

/**
 * Prints the binary representation of a 64-bit integer.
 *
 * @param value The integer whose bits are to be printed.
 */
static inline void
print_bits (uint64_t value)
{
  printf ("Bits: ");
  for (int i = 63; i >= 0; i--)
    {                                  /* Start from MSB to LSB */
      printf ("%d", (value >> i) & 1); /* Shift and mask the current bit */
      if (i % 8 == 0)
        printf (" "); /* Optional: Add space every byte */
    }
  printf ("\n");
}

#endif /* UTILS_H */

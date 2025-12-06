/* 
 * File: utils.c
 * Purpose: Implements the trimming for strings for the format string 
*/

/*
  Created by Viktor Liljenberg on 2024-12-19.
*/

#include "utils.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *
trim_whitespace (const char *str)
{
  if (!str)
    {
      return NULL;
    }

  /* we trim the starting space */
  const char *start = str;
  while (isspace ((unsigned char)*start))
    {
      start++;
    }

  /* If Empty or only white space we return an empty string */
  if (*start == '\0')
    {
      char *empty_str = (char *)malloc (1);
      if (!empty_str)
        {
          return NULL;
        }
      empty_str[0] = '\0'; /* Return an empty string */
      return empty_str;
    }

  /* Trim the trailing space */
  const char *end = start + strlen (start) - 1;
  while (end > start && isspace ((unsigned char)*end))
    {
      end--;
    }

  /* lets calc lenght for alloc */
  size_t trimmed_length = end - start + 1;

  /* lets allocate memory for the trimmed string (+1 for null terminator) */
  char *trimmed_str = (char *)malloc (trimmed_length + 1);
  if (!trimmed_str)
    {
      return NULL; /* Handles malloc failure since they apparently can fail? */
    }

  strncpy (trimmed_str, start, trimmed_length);
  trimmed_str[trimmed_length] = '\0'; /* add null termination */

  return trimmed_str;
}

/* remove all spaces in string "100 100" => "100100" */
char *
remove_all_spaces (const char *str)
{
  /* check string is some */
  if (!str)
    {
      return NULL;
    }

  /* how long is str */
  size_t length = strlen (str);
  /* allocate mem for str */
  char *no_spaces_str = (char *)malloc (length + 1); /* +1 for \0 */
  /* check allocation was ok */
  if (!no_spaces_str)
    {
      return NULL;
    }

  size_t j = 0;
  /* for the str lets only copy over the chars that is not space */
  for (size_t i = 0; i < length; i++)
    {
      if (!isspace ((unsigned char)str[i]))
        {
          no_spaces_str[j++] = str[i];
        }
    }
  no_spaces_str[j] = '\0'; /* add null terminator */

  return no_spaces_str;
}

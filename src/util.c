#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void print_hex(const unsigned char *str, size_t size)
{
  size_t c = 0;
  while(c < size) {
    printf("0x%02x ", *(str + c));

    c++;

    if(0 == (c % 8))
      printf("\n");
  }

  /* print one more new line if the last line
     has a number of hex not equal to 8 */
  if(c % 8)
    printf("\n");
}

const unsigned char *trim_space(char *str)
{
	char *start;
	char *cp = str;

  /* skip all leading spaces */
  while(*cp != '\0' && *cp == ' ')
    cp++;
  
  start = cp;

  /* copy the rest of string */
  while(*cp != '\0') {
    *str = *cp;
    str++; cp++;
  }
  *str = *cp;

  /* /\* move to the end of string *\/ */
  /* while(*cp == '\0') */
  /*   cp++; */

  /* skip all spaces from the end */
  cp = str - 1;
  while(*cp == ' ')
    cp--;

  /* set null terminate next to the last non-space char */
  *(cp + 1) = '\0';

  return start;
}

const char *reduce_mulspace(char *str)
{
  
}

#include "json.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_SIZE (64 * 1024)

int main(void) {
  char input[MAX_INPUT_SIZE + 1];
  size_t input_size = fread(input, 1, sizeof(input), stdin);
  if (ferror(stdin) || input_size > MAX_INPUT_SIZE) {
    return 2;
  }
  if (input_size == 0 || memchr(input, '\0', input_size) != NULL) {
    return 1;
  }

  json_value* value = json_parse(input, input_size);
  if (value == NULL) {
    return 1;
  }
  json_value_free(value);
  return 0;
}

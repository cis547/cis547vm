#include "vendor/json-parser/json.h"

#include <stdio.h>
#include <stdlib.h>

int main(void) {
  char file_contents[65536];
  size_t file_size = fread(file_contents, 1, sizeof(file_contents), stdin);

  if (ferror(stdin)) {
    return 1;
  }

  json_value* value = json_parse((json_char*)file_contents, file_size);

  if (value == NULL) {
    fprintf(stderr, "Unable to parse data\n");
    exit(0);
  }

  json_value_free(value);
  return 0;
}

/* Build the pinned parser in this translation unit so the student's LLVM
 * pass and Clang source-coverage instrumentation both see the complete
 * benchmark. */
#include "vendor/json-parser/json.c"

#include <stdio.h>
#include <stdlib.h>
#include "ir.h"

int main(void) {
  ir *t = ir_empty(sizeof(char));
  if (t == NULL) {
    return EXIT_FAILURE;
  }
  ir_dispose(&t);
  return EXIT_SUCCESS;
}

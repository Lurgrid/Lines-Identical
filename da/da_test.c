#include <stdio.h>
#include <stdlib.h>
#include "da.h"

int main(void) {
  da *t = da_empty(sizeof(char));
  if (t == NULL) {
    return EXIT_FAILURE;
  }
  da_dispose(&t);
  return EXIT_SUCCESS;
}

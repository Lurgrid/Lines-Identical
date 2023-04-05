#include <stdlib.h>
#include <stdio.h>
#define MAX 20000000

int main(void) {
  for (long int k = 0; k < MAX; ++k) {
    for (long int j = 256; j > 0; --j) {
      putchar('0' + rand() % 10);
    }
    putchar('\n');
  }
  return EXIT_SUCCESS;
}

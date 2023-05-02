#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define MAX 500000
#define LEN 8000

int main(void) {
  char alp[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
      ",;:!?./*-+@&\"'(-_)=+<> ";
  size_t alp_len = strlen(alp);
  srand((unsigned int) time(NULL));
  for (long int k = 0; k < MAX; ++k) {
    for (long int j = 0; j < LEN; ++j) {
      putchar(alp[(size_t) ((float) rand() / (float) RAND_MAX * (float) alp_len)]);
    }
    putchar('\n');
  }
  return EXIT_SUCCESS;
}

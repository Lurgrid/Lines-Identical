#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define MAX 2000
#define LEN 20000

static size_t str_hashfun(const char *s);
static size_t np2(size_t n);

int main(void) {
  char alp[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
      ",;:!?./*-+@&\"'(-_)=+<> ";
  size_t alp_len = strlen(alp);
  char ss[MAX][LEN + 1];
  size_t n = np2(MAX);
  srand((unsigned int) time(NULL));
  for (size_t i = 0; i < MAX; ++i) {
    for (size_t j = 0; j < LEN; ++j) {
      ss[i][j] = alp[i % alp_len];
    }
    ss[i][LEN] = '\0';
  }
  for (size_t i = 0; i < MAX; ++i) {
    printf("%zu\n", str_hashfun(ss[i]) % n);
  }
  size_t hash_val = str_hashfun(ss[0]);
  for (size_t i = 0; i < MAX; ++i) {
    if (str_hashfun(ss[i]) != hash_val) {
      fprintf(stderr,
          "*** Error: La collision de hachage n'a pas été trouvée\n%s\n",
          ss[i]);
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}

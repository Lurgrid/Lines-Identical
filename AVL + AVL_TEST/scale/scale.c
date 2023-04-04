#include "scale.h"
#include <limits.h>
#include <stdint.h>
#include <inttypes.h>
#include <errno.h>

struct scale {
  uint16_t valmax;
  uint16_t *array;
};

scale *scale_open(int valmax) {
  if (valmax > UINT16_MAX) {
    return NULL;
  }
  scale *s = malloc(sizeof *s);
  if (s == NULL) {
    return NULL;
  }
  s->valmax = (uint16_t) valmax;
  s->array = malloc((uint16_t) valmax * sizeof(uint16_t));
  if (s->array == NULL) {
    free(s);
    return NULL;
  }
  return s;
}

void scale_close(scale **sptr) {
  if (*sptr == NULL) {
    return;
  }
  free((*sptr)->array);
  free(*sptr);
  *sptr = NULL;
}

void scale_rand(scale *s, const void **pp) {
  size_t n = (size_t) (rand() % (s->valmax + 1));
  *(s->array + n) = (uint16_t) n;
  *pp = s->array + n;
}

int scale_get(scale *s, const void **pp) {
  long long int n;
  errno = 0;
  int r = scanf("%lld", &n);
  if (r == EOF) {
    return EOF;
  }
  if (r == 0 || errno != 0 || n < 0 || n > s->valmax) {
    return 0;
  }
  *(s->array + n) = (uint16_t) n;
  *pp = s->array + n;
  return 1;
}

void scale_value(const void *p, int *valptr) {
  *valptr = *(uint16_t *) p;
}

int scale_compar(const void *p1, const void *p2) {
  return (p1 > p2) - (p1 < p2);
}

//  Partie implantation du module da.

#include "da.h"
#include <string.h>
#include <stdint.h>

#define DA_MIN_LENGTH 1
#define DA_MUL_LENGTH 2

struct da {
  void *ref;
  size_t size;
  size_t capacity;
  size_t nmemb;
};

da *da_empty(size_t size) {
  da *d = malloc(sizeof(*d));
  if (d == NULL) {
    return NULL;
  }
  d->ref = malloc(size * DA_MIN_LENGTH);
  if (d->ref == NULL) {
    free(d);
    return NULL;
  }
  d->size = size;
  d->capacity = DA_MIN_LENGTH;
  d->nmemb = 0;
  return d;
}

void da_dispose(da **dptr) {
  if (*dptr != NULL) {
    free((*dptr)->ref);
    free(*dptr);
    *dptr = NULL;
  }
}

void *da_add(da *d, const void *ref) {
  if (ref == NULL) {
    return NULL;
  }
  if (d->nmemb == d->capacity) {
    if (d->nmemb >= SIZE_MAX / d->size / DA_MUL_LENGTH) {
      return NULL;
    }
    void *p = realloc(d->ref, d->size * d->nmemb * DA_MUL_LENGTH);
    if (p == NULL) {
      return NULL;
    }
    d->ref = p;
    d->capacity *= DA_MUL_LENGTH;
  }
  memcpy((char *) d->ref + d->nmemb * d->size, ref, d->size);
  ++d->nmemb;
  return (void *) ref;
}

void *da_cond_left_search(da *d, bool (*cond)(const void *)) {
  for (const char *p = (char *) d->ref;
      p < (char *) d->ref + d->size * d->nmemb; p += d->size) {
    if (cond(p)) {
      return (void *) p;
    }
  }
  return NULL;
}

void *da_nth(da *d, size_t n) {
  if (n >= d->nmemb) {
    return NULL;
  }
  return (void *)((char *)d->ref + n * d->size);
}

size_t da_length(da *d) {
  return d->nmemb;
}

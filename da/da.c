//  Partie implantation du module da.

#include "da.h"
#include <stdint.h>
#include <string.h>

//--- Macro da -----------------------------------------------------------------

#define DA_MIN_LENGTH 1
#define DA_MUL_LENGTH 2

//--- Définition da ------------------------------------------------------------

struct da {
  void *ref;
  size_t size;
  size_t capacity;
  size_t nmemb;
};

//--- Implémentation da --------------------------------------------------------

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

void *da_nth(da *d, size_t n) {
  if (n >= d->nmemb) {
    return NULL;
  }
  return (void *)((char *)d->ref + n * d->size);
}

size_t da_length(da *d) {
  return d->nmemb;
}

void da_reset(da *d) {
  d->nmemb = 0;
}


//  Partie implantation du module da.

#include "da.h"
#include <string.h>
#include <stdint.h>

#include <stdio.h>

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

int da_apply_context(da *d, void *cntxt, int (*fun)(void *, void *)) {
  int r = 0;
  for (char *p = (char *) d->ref;
    p < (char *) d->ref + d->size * d->nmemb; p += d->size) {
    if (fun(p, cntxt) != 0) {
      r = -1;
    }
  }
  return r;
}

int da_apply(da *d, int (*fun)(void *)) {
  int r = 0;
  for (char *p = (char *) d->ref;
    p < (char *) d->ref + d->size * d->nmemb; p += d->size) {
    if (fun(p) != 0) {
      r = -1;
    }
  }
  return r;
}

void da_reset(da *d) {
  d->nmemb = 0;
}

int da_cmp(da *d, da *b, int (*comp)(const void *, const void *)) {
  char *p = (char *) d->ref;
  char *q = (char *) b->ref;
  size_t dnmemb = d->nmemb;
  size_t bnmemb = b->nmemb;
  while (dnmemb != 0) {
    if (bnmemb == 0) {
      return 1;
    }
    int c = comp(p, q);
    if (c != 0) {
      return c;
    }
    p += d->size;
    q += b->size;
    --dnmemb;
    --bnmemb;
  }
  return bnmemb == 0 ? 0 : -1;
}

da *da_dupli(da *d) {
  da *c = malloc(sizeof(*d));
  if (c == NULL) {
    return NULL;
  }
  c->ref = malloc(d->size * d->nmemb);
  if (c->ref == NULL) {
    free(c);
    return NULL;
  }
  c->size = d->size;
  c->nmemb = d->nmemb;
  c->capacity = c->nmemb;
  memcpy(c->ref, d->ref, d->size * d->nmemb);
  return c;
}

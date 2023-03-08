//  Partie implantation du module ir.

#include "ir.h"
#include <string.h>
#include <stdint.h>

#define IR_MIN_LENGTH 1
#define IR_MUL_LENGTH 2

struct ir {
  void *ref;
  size_t size;
  size_t capacity;
  size_t nmemb;
};

ir *ir_empty(size_t size) {
  ir *a = malloc(sizeof(*a));
  if (a == NULL) {
    return NULL;
  }
  a->ref = malloc(size * IR_MIN_LENGTH);
  if (a->ref == NULL) {
    free(a);
    return NULL;
  }
  a->size = size;
  a->capacity = IR_MIN_LENGTH;
  a->nmemb = 0;
  return a;
}

void ir_dispose(ir **irptr) {
  if (*irptr != NULL) {
    free((*irptr)->ref);
    free(*irptr);
    *irptr = NULL;
  }
}

void *ir_add(ir *a, const void *ref) {
  if (ref == NULL) {
    return NULL;
  }
  if (a->nmemb == a->capacity) {
    if (a->nmemb >= SIZE_MAX / a->size / IR_MUL_LENGTH) {
      return NULL;
    }
    void *p = realloc(a->ref, a->size * a->nmemb * IR_MUL_LENGTH);
    if (p == NULL) {
      return NULL;
    }
    a->ref = p;
    a->capacity *= IR_MUL_LENGTH;
  }
  memcpy((char *) a->ref + a->nmemb * a->size, ref, a->size);
  ++a->nmemb;
  return (void *) ref;
}

void *ir_cond_left_search(ir *a, bool (*cond)(const void *)) {
  for (const char *p = (char *) a->ref;
      p < (char *) a->ref + a->size * a->nmemb; p += a->size) {
    if (cond(p)) {
      return (void *) p;
    }
  }
  return NULL;
}

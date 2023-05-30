//  Partie implantation du module holdall.

#include "holdall.h"

//  struct holdall, holdall : implantation par liste dynamique simplement
//    chainée.

//  Si la macroconstante HOLDALL_PUT_TAIL est définie et que sa macro-évaluation
//    donne une entier non nul, l'insertion dans la liste a lieu en queue. Dans
//    le cas contraire, elle a lieu en tête.

typedef struct choldall choldall;

struct choldall {
  void *ref;
  choldall *next;
};

struct holdall {
  choldall *head;
#if defined HOLDALL_PUT_TAIL && HOLDALL_PUT_TAIL != 0
  choldall **tailptr;
#endif
  size_t count;
};

holdall *holdall_empty(void) {
  holdall *ha = malloc(sizeof *ha);
  if (ha == NULL) {
    return NULL;
  }
  ha->head = NULL;
#if defined HOLDALL_PUT_TAIL && HOLDALL_PUT_TAIL != 0
  ha->tailptr = &ha->head;
#endif
  ha->count = 0;
  return ha;
}

void holdall_dispose(holdall **haptr) {
  if (*haptr == NULL) {
    return;
  }
  choldall *p = (*haptr)->head;
  while (p != NULL) {
    choldall *t = p;
    p = p->next;
    free(t);
  }
  free(*haptr);
  *haptr = NULL;
}

int holdall_put(holdall *ha, void *ref) {
  choldall *p = malloc(sizeof *p);
  if (p == NULL) {
    return -1;
  }
  p->ref = ref;
#if defined HOLDALL_PUT_TAIL && HOLDALL_PUT_TAIL != 0
  p->next = NULL;
  *ha->tailptr = p;
  ha->tailptr = &p->next;
#else
  p->next = ha->head;
  ha->head = p;
#endif
  ha->count += 1;
  return 0;
}

size_t holdall_count(holdall *ha) {
  return ha->count;
}

int holdall_apply(holdall *ha,
    int (*fun)(void *)) {
  for (const choldall *p = ha->head; p != NULL; p = p->next) {
    int r = fun(p->ref);
    if (r != 0) {
      return r;
    }
  }
  return 0;
}

int holdall_apply_context(holdall *ha,
    void *context, void *(*fun1)(void *context, void *ptr),
    int (*fun2)(void *ptr, void *resultfun1)) {
  for (const choldall *p = ha->head; p != NULL; p = p->next) {
    int r = fun2(p->ref, fun1(context, p->ref));
    if (r != 0) {
      return r;
    }
  }
  return 0;
}

int holdall_apply_context2(holdall *ha,
    void *context1, void *(*fun1)(void *context1, void *ptr),
    void *context2, int (*fun2)(void *context2, void *ptr, void *resultfun1)) {
  for (const choldall *p = ha->head; p != NULL; p = p->next) {
    int r = fun2(context2, p->ref, fun1(context1, p->ref));
    if (r != 0) {
      return r;
    }
  }
  return 0;
}

#if defined HOLDALL_WANT_EXT && HOLDALL_WANT_EXT != 0

void choldall__merge(choldall **sptr, choldall *p, choldall *q, size_t nmembp,
    size_t nmembq, int (*compar)(const void *, const void *)) {
  if (nmembp == 0) {
    *sptr = q;
    return;
  }
  if (nmembq == 0) {
    *sptr = p;
    return;
  }
  if (compar(p->ref, q->ref) > 0) {
    *sptr = q;
    q = q->next;
    choldall__merge(&(*sptr)->next, p, q, nmembp, nmembq - 1, compar);
    return;
  }
  *sptr = p;
  p = p->next;
  choldall__merge(&(*sptr)->next, p, q, nmembp - 1, nmembq, compar);
}

void choldall__split(choldall *h, choldall **p, choldall **q, size_t nmemb) {
  choldall *ph = h;
  *p = ph;
  for (size_t i = 1; i < nmemb - (nmemb / 2); ++i) {
    ph = ph->next;
  }
  *q = ph->next;
  ph->next = NULL;
  //ph = *q;
  //for (size_t i = 1; i < nmemb / 2; ++i) {
    //ph = ph->next;
  //}
  //ph->next = NULL;
}

void choldall__sort(choldall **h, size_t nmemb, int (*compar)(const void *,
    const void *)) {
  if (nmemb == 1) {
    return;
  }
  choldall *p;
  choldall *q;
  choldall__split(*h, &p, &q, nmemb);
  choldall__sort(&p, nmemb - (nmemb / 2), compar);
  choldall__sort(&q, nmemb / 2, compar);
  choldall__merge(h, p, q, nmemb - (nmemb / 2), nmemb / 2, compar);
}

void holdall_sort(holdall *ha, int (*compar)(const void *, const void *)) {
  if (ha->count < 2) {
    return;
  }
  choldall__sort(&ha->head, ha->count, compar);
}

#endif

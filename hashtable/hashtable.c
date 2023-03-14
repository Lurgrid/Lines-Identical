//  hashtable.c : partie implantation d'un module polymorphe pour la
//    spécification TABLE du TDA Table(T, T') dans le cas d'une table de hachage
//    par chainage séparé.

#include <stdint.h>
#include "hashtable.h"

//  Le nombre de compartiments du tableau de hachage est une puissance de 2. Il
//    vaut initialement « 2 ^ HT__LBNSLOTS_MIN ». Dès que le taux de remplissage
//    de la table de hachage est strictement supérieur à
//    « (double) HT__LDFACT_MAX_NUMER / (double) HT__LDFACT_MAX_DENOM », le
//    nombre de compartiments est multiplié par 2.

#define HT__LBNSLOTS_MIN      6
#define HT__LDFACT_MAX_NUMER  1
#define HT__LDFACT_MAX_DENOM  1

//  Les définitions précédentes vont pour un nombre de compartiments initial de
//    64 et un seuil maximum de 1.0 ; ces définitions peuvent être modifiées.
//    Les directives qui suivent s'assurent de leur cohérence ; ces directives
//    ne doivent pas être modifiées.

#define HT__NSLOTS_MIN \
  (1ULL << HT__LBNSLOTS_MIN)
#define HT__NENTRIESMAX_MIN \
  (HT__NSLOTS_MIN / HT__LDFACT_MAX_DENOM * HT__LDFACT_MAX_NUMER)

#if HT__LBNSLOTS_MIN < 0                                                       \
  || HT__LDFACT_MAX_NUMER < 0                                                  \
  || HT__LDFACT_MAX_DENOM < 1                                                  \
  || HT__NSLOTS_MIN == 0                                                       \
  || HT__NSLOTS_MIN > SIZE_MAX                                                 \
  || HT__NENTRIESMAX_MIN == 0
#error Bad choice of HT__ constants.
#endif

#undef HT__NSLOTS_MIN
#undef HT__NENTRIESMAX_MIN

//  struct hashtable, hashtable : gestion du chainage séparé par liste dynamique
//    simplement chainée. Le composant compar mémorise la fonction de
//    comparaison des clés, hashfun, leur fonction de pré-hachage. Le tableau de
//    hachage est alloué dynamiquement ; son adresse et le logarithme binaire de
//    sa longueur, autrement dit : le nombre de compartiments, sont mémorisés
//    par les composants hasharray et lbnslots. Le composant nfreeentries
//    mémorise le nombre d'entrées libres, autrement dit : la différence, dans
//    la configuration courante de la table de hachage, entre le nombre
//    d'entrées maximales associées au seuil et le nombre d'entrées. Le champ
//    null a un double usage dès lors que la table de hachage a été
//    initialisée : 1) tant que le tableau de hachage n'a pas été alloué,
//    la valeur de hasharray est l'adresse du champ null ; 2) la fonction de
//    recherche locale hashtable__search est toujours définie car la valeur du
//    champ null est NULL.

//  L'ajout d'une nouvelle entrée a lieu en queue de liste. L'ordre induit est
//    respecté lors de tout agrandissement du tableau de hachage.

typedef struct cell cell;

struct cell {
  const void *keyref;
  const void *valref;
  cell *next;
};

struct hashtable {
  int (*compar)(const void *, const void *);
  size_t (*hashfun)(const void *);
  cell **hasharray;
  cell *null;
  size_t lbnslots;
  size_t nfreeentries;
};

#define HT__MAKE_BLANK(ht)  ((ht)->hasharray = &(ht)->null)
#define HT__IS_BLANK(ht)    ((ht)->hasharray == &(ht)->null)

#define HALF(k) ((k) >> 1)
#define POW2(n) ((size_t) 1 << (n))

#define HASHVAL(__hashfun, __lbnslots, __keyref)                               \
  (__hashfun(__keyref) % POW2(__lbnslots))

//  hashtable__search : recherche dans la table de hachage associé à ht une clé
//    égale à keyref au sens de compar. Renvoie l'adresse du pointeur qui repère
//    la cellule qui contient cette occurrence si elle existe. Renvoie sinon
//    l'adresse du pointeur qui marque la fin de la liste.
static cell **hashtable__search(const hashtable *ht, const void *keyref) {
  size_t k = HASHVAL(ht->hashfun, ht->lbnslots, keyref);
  cell * const *pp = &ht->hasharray[k];
  while (*pp != NULL && ht->compar(keyref, (*pp)->keyref) != 0) {
    pp = &(*pp)->next;
  }
  return (cell **) pp;
}

//  hashtable__add_enlarge : initialise ou agrandit le tableau de hachage de la
//    table de hachage associée à ht. Il est supposé que la valeur de
//    nfreeentries est nulle. Renvoie une valeur non nulle en cas de dépassement
//    de capacité. Renvoie sinon zéro.
static int hashtable__add_enlarge(hashtable *ht) {
  int b;
  size_t lbm;
  size_t m;
  size_t m_;
  if ((b = HT__IS_BLANK(ht))) {
    lbm = HT__LBNSLOTS_MIN;
    m = POW2(lbm);
    m_ = 0;
    ht->hasharray = NULL;
  } else {
    lbm = ht->lbnslots + 1;
    m = POW2(lbm);
    m_ = HALF(m);
  }
  cell **a;
  if (m > SIZE_MAX / sizeof *a
      || (HT__LDFACT_MAX_NUMER > sizeof *a
      && HT__LDFACT_MAX_NUMER > HT__LDFACT_MAX_DENOM
      && m > SIZE_MAX / HT__LDFACT_MAX_NUMER * HT__LDFACT_MAX_DENOM)
      || (a = realloc(ht->hasharray, m * sizeof *a)) == NULL) {
    if (b) {
      HT__MAKE_BLANK(ht);
    }
    return -1;
  }
  if (b) {
    for (size_t k = 0; k < m; ++k) {
      a[k] = NULL;
    }
  } else {
    for (size_t k_ = 0; k_ < m_; ++k_) {
      cell **pp_ = &a[k_];
      cell **pp = &a[k_ + m_];
      while (*pp_ != NULL) {
        if (HASHVAL(ht->hashfun, lbm, (*pp_)->keyref) < m_) {
          pp_ = &(*pp_)->next;
        } else {
          *pp = *pp_;
          *pp_ = (*pp_)->next;
          pp = &(*pp)->next;
        }
      }
      *pp = NULL;
    }
  }
  ht->hasharray = a;
  ht->lbnslots = lbm;
  ht->nfreeentries
    = m / HT__LDFACT_MAX_DENOM * HT__LDFACT_MAX_NUMER
      - m_ / HT__LDFACT_MAX_DENOM * HT__LDFACT_MAX_NUMER;
  return 0;
}

hashtable *hashtable_empty(int (*compar)(const void *, const void *),
    size_t (*hashfun)(const void *)) {
  hashtable *ht = malloc(sizeof *ht);
  if (ht == NULL) {
    return NULL;
  }
  ht->compar = compar;
  ht->hashfun = hashfun;
  HT__MAKE_BLANK(ht);
  ht->null = NULL;
  ht->lbnslots = 0;
  ht->nfreeentries = 0;
  return ht;
}

void hashtable_dispose(hashtable **htptr) {
  if (*htptr == NULL) {
    return;
  }
  if (!HT__IS_BLANK(*htptr)) {
    size_t m = POW2((*htptr)->lbnslots);
    for (size_t k = 0; k < m; ++k) {
      cell *p = (*htptr)->hasharray[k];
      while (p != NULL) {
        cell *t = p;
        p = p->next;
        free(t);
      }
    }
    free((*htptr)->hasharray);
  }
  free(*htptr);
  *htptr = NULL;
}

void *hashtable_add(hashtable *ht, const void *keyref, const void *valref) {
  if (valref == NULL) {
    return NULL;
  }
  cell **pp = hashtable__search(ht, keyref);
  if (*pp != NULL) {
    const void *r = (*pp)->valref;
    (*pp)->valref = valref;
    return (void *) r;
  }
  if (ht->nfreeentries == 0) {
    if (hashtable__add_enlarge(ht) != 0) {
      return NULL;
    }
    pp = hashtable__search(ht, keyref);
  }
  cell *p = malloc(sizeof *p);
  if (p == NULL) {
    return NULL;
  }
  p->keyref = keyref;
  p->valref = valref;
  p->next = *pp;
  *pp = p;
  ht->nfreeentries -= 1;
  return (void *) valref;
}

void *hashtable_remove(hashtable *ht, const void *keyref) {
  cell **pp = hashtable__search(ht, keyref);
  if (*pp == NULL) {
    return NULL;
  }
  cell *p = *pp;
  const void *r = p->valref;
  *pp = p->next;
  free(p);
  ht->nfreeentries += 1;
  return (void *) r;
}

void *hashtable_search(hashtable *ht, const void *keyref) {
  const cell *p = *hashtable__search(ht, keyref);
  return p == NULL ? NULL : (void *) p->valref;
}

#if defined HASHTABLE_STATS && HASHTABLE_STATS != 0

void hashtable_get_stats(hashtable *ht,
    struct hashtable_stats *htsptr) {
  size_t m = (HT__IS_BLANK(ht) ? 0 : POW2(ht->lbnslots));
  size_t n = m / HT__LDFACT_MAX_DENOM * HT__LDFACT_MAX_NUMER - ht->nfreeentries;
  size_t g = 0;
  double s = 0.0;
  for (size_t k = 0; k < m; ++k) {
    size_t f = 0;
    const cell *p = ht->hasharray[k];
    while (p != NULL) {
      ++f;
      p = p->next;
    }
    if (f > g) {
      g = f;
    }
    s += (double) f * (double) (f + 1) / 2.0;
  }
  double r = (double) n / (double) m;
  *htsptr = (struct hashtable_stats) {
    .nslots = m,
    .nentries = n,
    .ldfactmax = (double) HT__LDFACT_MAX_NUMER / (double) HT__LDFACT_MAX_DENOM,
    .ldfactcurr = r,
    .maxlen = g,
    .postheo = (n == 0 ? 0.0 : 1.0 + (r - 1.0 / (double) m) / 2.0),
    .poscurr = s / (double) n,
  };
}

#define P_TITLE(textstream, name) \
  fprintf(textstream, "--- Info: %s\n", name)
#define P_VALUE(textstream, name, format, value) \
  fprintf(textstream, "%12s\t" format "\n", name, value)

int hashtable_fprint_stats(hashtable *ht, FILE *textstream) {
  struct hashtable_stats hts;
  hashtable_get_stats(ht, &hts);
  return 0 > P_TITLE(textstream, "Hashtable stats")
    || 0 > P_VALUE(textstream, "n.slots", "%zu", hts.nslots)
    || 0 > P_VALUE(textstream, "n.entries", "%zu", hts.nentries)
    || 0 > P_VALUE(textstream, "ld.fact.max", "%lf", hts.ldfactmax)
    || 0 > P_VALUE(textstream, "ld.fact.curr", "%lf", hts.ldfactcurr)
    || 0 > P_VALUE(textstream, "max.len", "%zu", hts.maxlen)
    || 0 > P_VALUE(textstream, "pos.theo", "%lf", hts.postheo)
    || 0 > P_VALUE(textstream, "pos.curr", "%lf", hts.poscurr);
}

#endif

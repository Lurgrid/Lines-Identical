//  bst.c : partie implantation d'un module polymorphe pour la spécification
//   ABINR du TDA ABinR(T).

#include "bst.h"

//=== Type cbst ================================================================

//--- Définition cbst ----------------------------------------------------------

typedef struct cbst cbst;

struct cbst {
  cbst *next[2];
  const void *value;
  int height;
};

//--- Raccourcis cbst ----------------------------------------------------------

#define EMPTY()     NULL
#define IS_EMPTY(p) ((p) == NULL)
#define LEFT(p)     ((p)->next[0])
#define RIGHT(p)    ((p)->next[1])
#define VALUE(p)    ((p)->value)
#define HEIGTH(p)    ((p)->height)
#define NEXT(p, d)  ((p)->next[(d) > 0])

//--- Divers -------------------------------------------------------------------

static size_t add__size_t(size_t x1, size_t x2) {
  return x1 + x2;
}

static int max__int(int x1, int x2) {
  return x1 > x2 ? x1 : x2;
}

static size_t min__size_t(size_t x1, size_t x2) {
  return x1 < x2 ? x1 : x2;
}

//--- Fonctions cbst -----------------------------------------------------------

//  DEFUN_CBST__MEASURE : définit la fonction récursive de nom « cbst__ ## fun »
//    et de paramètre un pointeur d'arbre binaire, qui renvoie zéro si l'arbre
//    est vide et « 1 + oper(r0, r1) » sinon, où r0 et r1 sont les valeurs
//    renvoyées par les appels récursifs de la fonction avec les pointeurs des
//    sous-arbres gauche et droit de l'arbre comme paramètres.
#define DEFUN_CBST__MEASURE(fun, oper)                                         \
  static size_t cbst__ ## fun(const cbst * p) {                                \
    return IS_EMPTY(p)                                                         \
      ? 0                                                                      \
      : 1 + oper(cbst__ ## fun(LEFT(p)), cbst__ ## fun(RIGHT(p)));             \
  }

//  cbst__size, cbst__distance : définition.

DEFUN_CBST__MEASURE(size, add__size_t)
DEFUN_CBST__MEASURE(distance, min__size_t)

//  cbst__height :  Renvoie la hauteur du sous-arbre associé à p.
static int cbst__height(const cbst *p) {
  return IS_EMPTY(p) ? 0 : HEIGTH(p);
}

//  cbst__update_height : Met a jour le champ height du sous-arbre associé à p.
static void cbst__update_height(cbst *p) {
  HEIGTH(p) = max__int(cbst__height(LEFT(p)), cbst__height(RIGHT(p))) + 1;
}

//  cbst__balance : Renvoie l'équilibre du sous-arbre associé à p.
static int cbst__balance(const cbst *p) {
  return cbst__height(LEFT(p)) - cbst__height(RIGHT(p));
}

//  cbst__rotation_left:
static void cbst__rotation_left(cbst **pp) {
  cbst *p = RIGHT(*pp);
  RIGHT(*pp) = LEFT(p);
  cbst__update_height(*pp);
  LEFT(p) = *pp;
  *pp = p;
  cbst__update_height(*pp);
}

//  cbst__rotation_right:
static void cbst__rotation_right(cbst **pp) {
  cbst *p = *pp;
  *pp = LEFT(p);
  LEFT(p) = RIGHT(*pp);
  cbst__update_height(p);
  RIGHT(*pp) = p;
  cbst__update_height(*pp);
}

//  cbst__rotation_left_right:
static void cbst__rotation_left_right(cbst **pp) {
  cbst__rotation_left(&LEFT(*pp));
  cbst__rotation_right(pp);
}

//  cbst__rotation_right_left:
static void cbst__rotation_right_left(cbst **pp) {
  cbst__rotation_right(&RIGHT(*pp));
  cbst__rotation_left(pp);
}

//  cbst__balancing:
static int cbst__balancing(cbst **pp) {
  cbst__update_height(*pp);
  int b = cbst__balance(*pp);
  if (b <= 1 && b >= -1) {
    return 0;
  }
  if (b == 2) {
    if (cbst__balance(LEFT(*pp)) >= 0) {
      cbst__rotation_right(pp);
    } else {
      cbst__rotation_left_right(pp);
    }
  } else {
    if (cbst__balance(RIGHT(*pp)) <= 0) {
      cbst__rotation_left(pp);
    } else {
      cbst__rotation_right_left(pp);
    }
  }
  return -1;
}

//  cbt__dispose : libère les ressources allouées à l'arbre binaire pointé par
//    p.
static void cbst__dispose(cbst *p) {
  if (!IS_EMPTY(p)) {
    cbst__dispose(LEFT(p));
    cbst__dispose(RIGHT(p));
    free(p);
  }
}

//  cbst__remove_max : Renvoie NULL si *pp est un arbre vide, sinon remplace la
//    valeur du noeud *pp par la valeur maximum du sous-arbre gauche pointé par
//    *pp.
//    Renvoie la référence de la valeur de *pp.
static void *cbst__remove_max(cbst **pp) {
  if (IS_EMPTY(*pp)) {
    return NULL;
  }
  if (IS_EMPTY(RIGHT(*pp))) {
    const void *m = VALUE(*pp);
    *pp = LEFT(*pp);
    return (void *) m;
  }
  const void *value = cbst__remove_max(&RIGHT(*pp));
  cbst__balancing(pp);
  return (void *) value;
}

//  cbst__remove_root : Renvoie NULL si *pp est un arbre vide, sinon supprime la
//    racine de l'arbre *pp par la méthode par remonté à gauche.
//    Renvoie la valeur du noeud *pp.
static void *cbst__remove_root(cbst **pp) {
  if (IS_EMPTY(*pp)) {
    return NULL;
  }
  const void *v = VALUE(*pp);
  if (IS_EMPTY(LEFT(*pp))) {
    cbst *p = *pp;
    *pp = RIGHT(*pp);
    free(p);
    return (void *) v;
  }
  VALUE(*pp) = cbst__remove_max(&LEFT(*pp));
  cbst__balancing(pp);
  return (void *) v;
}

//  cbst__remove_climbup_left : recherche dans l'arbre binaire associé à *pp la
//    référence d'un objet égal à celle de référence ref au sens de la fonction
//    de comparaison. Si la recherche est négative, renvoie NULL.
//    Retire sinon la référence trouvée selon la méthode du retrait par remontée
//    gauche et renvoie la référence trouvée.
static void *cbst__remove_climbup_left(cbst **pp, const void *ref,
    int (*compar)(const void *, const void *)) {
  if (IS_EMPTY(*pp)) {
    return NULL;
  }
  int r = compar(ref, VALUE(*pp));
  if (r == 0) {
    return cbst__remove_root(pp);
  }
  const void *v = cbst__remove_climbup_left(&NEXT(*pp, r), ref, compar);
  cbst__balancing(pp);
  return (void *) v;
}

//  cbst__add_endofpath : L'utilisation de cette fonction implique que ref est
//    différent de NULL. Recherche dans l'arbre pointé par *pp si une occurence
//    de ref apparait. Si oui la renvoie, sinon ajoute ref en utilisisant la
//    méthode d'ajout en bout de chemin.
//    Renvoie NULL en cas de dépasement de capacité ; sinon renvoie ref.
static void *cbst__add_endofpath(cbst **pp, const void *ref,
    int (*compar)(const void *, const void *)) {
  if (IS_EMPTY(*pp)) {
    *pp = malloc(sizeof **pp);
    if (*pp == NULL) {
      return NULL;
    }
    VALUE(*pp) = ref;
    LEFT(*pp) = NULL;
    RIGHT(*pp) = NULL;
    HEIGTH(*pp) = 1;
    return (void *) ref;
  }
  int r = compar(ref, VALUE(*pp));
  if (r == 0) {
    return (void *) VALUE(*pp);
  }
  const void *v = cbst__add_endofpath(&NEXT(*pp, r), ref, compar);
  cbst__balancing(pp);
  return (void *) v;
}

//  cbst__search : recherche dans l'abl pointé par p, une référence égal à ref
//    selon la fonction compar.
//    Renvoi NULL si aucune des réréfence de n'est égal à ref selon compar,
//    sinon renvoie la référence trouver.
static void *cbst__search(const cbst *p, const void *ref,
    int (*compar)(const void *, const void *)) {
  if (IS_EMPTY(p)) {
    return NULL;
  }
  int r = compar(ref, VALUE(p));
  if (r == 0) {
    return (void *) VALUE(p);
  }
  return cbst__search(NEXT(p, r), ref, compar);
}

//  cbst__number : calcule le numéro de l'arbre associé à t dont la valeur est
//    égale à celle de l'objet pointé par ref au sens de la fonction de
//    comparaison.
//    Renvoie ce numéro + number. Renvoie sinon le numéro + number qu'aurait le
//    nœud si la référence ref était ajoutée à l'arbre.
static size_t cbst__number(const cbst *p, const void *ref,
    int (*compar)(const void *, const void *), size_t number) {
  if (IS_EMPTY(p)) {
    return number;
  }
  int r = compar(ref, VALUE(p));
  if (r == 0) {
    return number;
  }
  if (r > 0) {
    return cbst__number(LEFT(p), ref, compar, 2 * number + 1);
  }
  return cbst__number(RIGHT(p), ref, compar, 2 * number);
}

//  cbst__rank : calcule le rang du nœud de l'arbre binaire associé à p dont la
//    valeur est égale à celle de l'objet pointé par ref au sens de la fonction
//    de comparaison.
//    Renvoie ce rang + rank si une telle valeur existe. Renvoie sinon le
//    rang + rank qu'aurait le nœud si la référence ref était ajoutée à l'arbre.
static size_t cbst__rank(const cbst *p, const void *ref,
    int (*compar)(const void *, const void *), size_t rank) {
  if (IS_EMPTY(p)) {
    return rank;
  }
  int r = compar(ref, VALUE(p));
  if (r == 0) {
    return rank;
  }
  if (r > 0) {
    return cbst__rank(RIGHT(p), ref, compar,
        rank + 1 + (IS_EMPTY(RIGHT(p)) ? 0 : cbst__size(LEFT(RIGHT(p)))));
  }
  return cbst__rank(LEFT(p), ref, compar,
      rank - 1 - (IS_EMPTY(LEFT(p)) ? 0 : cbst__size(RIGHT(LEFT(p)))));
}

#define REPR__TAB 4
#define REPR_SYM_EMPTY "│"
#define REPR_SYM_LINK_BETWEEN "│"
#define REPR_SYM_LINK_UP "╭"
#define REPR_SYM_LINK_DOWN "╰"
#define REPR_SYM_SEP "─"

#define GET_NBIT(m, n) ((1 << ((n) - 1)) & (m))

#define DISPLAY_BRANCH(num, level)                                             \
  for (int k = 1; k < level; ++k) {                                            \
    printf("%s",                                                               \
    (GET_NBIT(num, level - k)                                                  \
    && !GET_NBIT(num, level - k + 1))                                          \
    || (!GET_NBIT(num, level - k)                                              \
    && GET_NBIT(num, level - k + 1))                                           \
    ? REPR_SYM_LINK_BETWEEN : " ");                                            \
    printf("%*s", REPR__TAB - 1, "");                                          \
  }                                                                            \
  if (level > 0) {                                                             \
    printf("%s---", num % 2 ? REPR_SYM_LINK_UP : REPR_SYM_LINK_DOWN);          \
  }                                                                            \

//  cbst__repr_graphic_aux : affiche la représentation graphique par rotation
//    antihoraire d'un quart de tour du sous-arbre binaire valuer p. Pour chaque
//    noeud aillant une valeur, sa valeur est affiché suivie, du numéro du noeud,
//    de son rang et de son équilibre. Chaque niveau de l'arbre est séparer d'une
//    distance de REPR__TAB charactère. Les jointures entre noeuds sont les chaines
//    REPR_SYM_LINK_BETWEEN, REPR_SYM_LINK_UP, REPR_SYM_LINK_DOWN et REPR_SYM_SEP.
//    Le sous-arbre vide est représenter par la chaine pointé par REPR_SYM_EMPTY.
//    Le niveau du sous-arbre est supposé être level, le numéro num et son rang
//    correspond au résultat du calcule taille(sa-gauche(p)) + rank (avec taille
//    (sa-gauche(p)) la taille du sous arbre gauche de p).
static void cbst__repr_graphic_aux(const cbst *p, void (*put)(const void *ref),
    int level, size_t num, size_t rank) {
  while (!IS_EMPTY(p)) {
    cbst__repr_graphic_aux(RIGHT(p), put, level + 1, 2 * num + 1,
        cbst__size(LEFT(p)) + rank + 1);
    DISPLAY_BRANCH(num, level);
    put(p->value);
    printf(" n=%zu", num);
    printf(" h=%d r=%zu\n", cbst__height(p), cbst__size(LEFT(p)) + rank);
    p = LEFT(p);
    ++level;
    num *= 2;
  }
  DISPLAY_BRANCH(num, level);
  printf("%s n=%zu", REPR_SYM_EMPTY, num);
  putchar('\n');
  return;
}

//=== Type bst =================================================================

//--- Définition bst -----------------------------------------------------------

struct bst {
  int (*compar)(const void *, const void *);
  cbst *root;
};

//--- Fonctions bst ------------------------------------------------------------

bst *bst_empty(int (*compar)(const void *, const void *)) {
  bst *t = malloc(sizeof *t);
  if (t == NULL) {
    return NULL;
  }
  t->compar = compar;
  t->root = EMPTY();
  return t;
}

void bst_dispose(bst **tptr) {
  if (*tptr == NULL) {
    return;
  }
  cbst__dispose((*tptr)->root);
  free(*tptr);
  *tptr = NULL;
}

void *bst_remove_climbup_left(bst *t, const void *ref) {
  return cbst__remove_climbup_left(&(t->root), ref, t->compar);
}

void *bst_add_endofpath(bst *t, const void *ref) {
  if (ref == NULL) {
    return NULL;
  }
  return cbst__add_endofpath(&(t->root), ref, t->compar);
}

void *bst_search(bst *t, const void *ref) {
  return cbst__search(t->root, ref, t->compar);
}

size_t bst_size(bst *t) {
  return cbst__size(t->root);
}

int bst_height(bst *t) {
  return cbst__height(t->root);
}

size_t bst_distance(bst *t) {
  return cbst__distance(t->root);
}

size_t bst_number(bst *t, const void *ref) {
  return cbst__number(t->root, ref, t->compar, 1);
}

size_t bst_rank(bst *t, const void *ref) {
  if (IS_EMPTY(t)) {
    return 0;
  }
  return cbst__rank(t->root, ref, t->compar, cbst__size(LEFT(t->root)));
}

void bst_repr_graphic(bst *t, void (*put)(const void *ref)) {
  cbst__repr_graphic_aux(t->root, put, 0, 1, 0);
}

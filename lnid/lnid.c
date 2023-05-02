#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <locale.h>
#include "da.h"
#include "holdall.h"
#include "hashtable.h"
#include "optl.h"
#include "bst.h"

//--- Macro définissant les couleurs utilisables dans le terminal --------------

#define ANSI_RED    "\033[0;31m"
#define ANSI_GREEN  "\033[0;32m"
#define ANSI_YELLOW "\033[0;33m"
#define ANSI_NORM   "\033[0;0m"

//--- Macro d'affichage d'erreur -----------------------------------------------

#define ERROR(context, err)                                                    \
  fprintf(stderr, "%s*** Error: %s%s\n", (context).use_color ? ANSI_RED : "",  \
      (err), (context).use_color ? ANSI_NORM : "")                             \

#define ERROR2(context, err1, var, err2)                                       \
  fprintf(stderr, "%s*** Error: %s%s%s%s\n", (context).use_color ? ANSI_RED    \
      : "", (err1), (var), (err2), (context).use_color ? ANSI_NORM : "")       \

//--- Macro et structure utilisé pour les fonctions d'option -------------------

#define STD "standard"
#define LC "local"

#define ALNUM "alnum"
#define ALPHA "alpha"
#define BLANK "blank"
#define DIGIT "digit"
#define CNTRL "cntrl"
#define GRAPH "graph"
#define LOWER "lower"
#define PRINT "print"
#define PUNCT "punct"
#define SPACE "space"
#define UPPER "upper"
#define XDIGIT "xdigit"

struct category {
  const char *label;
  int (*cond)(int c);
};

//--- Structure de context -----------------------------------------------------

typedef struct {
  int (*filter)(int c);
  int (*transform)(int c);
  bool use_color;
  bool use_avl;
  int (*sort)(const void *, const void *);
  da *filesptr;
} cntxt;

//--- Structure du type de valeur présent les avl ------------------------------

typedef struct {
  const char *ref;
  da *count;
} hcell;

//--- Fonction de traitement d'option ------------------------------------------

//  HANDLE_PARAM_NO_ARG : définit la fonction de nom « fun ## _handler »
//    et de paramètre un pointeur sur un context de type cntxt, une chaine de
//    charactère value non utiliser et un pointeur vers une chaine de charactère
//    err. La fonction définie, change le champ attribut du context en lui
//    attribuant la valeur val, en méttant *err à NULL et en retournant 0.
#define HANDLE_PARAM_NO_ARG(fun, attribut, val)                                \
  static int fun ## _handler(cntxt * restrict context, __attribute__((unused)) \
    const char * restrict value, const char **err) {                           \
    *err = NULL;                                                               \
    context->attribut = val;                                                   \
    return 0;                                                                  \
  }

//  uppercasing_handler, no_color_handler, avl_handler: définition.

HANDLE_PARAM_NO_ARG(uppercasing, transform, toupper)
HANDLE_PARAM_NO_ARG(no_color, use_color, false)
HANDLE_PARAM_NO_ARG(avl, use_avl, true)

//  file_handler : Ajoute le nom du fichier pointer par filename, au champs
//    filesptr du context.
//    Retourne une valeur nul en cas de réussite, et met *err à la valeur NULL.
//    Sinon renvoie une valeur non nul, et la chaine pointer par *err représente
//    l'erreur.
static int file_handler(cntxt * restrict context,
    const char * restrict filename, const char **err);

//  sort_handler, filter_handler : Si value est un argument valide pour l'option
//    sort (resp filter) selon cette fonction, alors met context->sort
//    (resp context->filter) à la fonction adecoite.
//    Dans ce cas renvoie une valeur nulle et met *err à la valeur NULL. Sinon
//    renvois un entier non nulle et *err pointe sur le message correspondant à
//    l'erreur.
static int sort_handler(cntxt * restrict context,
    const char * restrict value, const char ** restrict err);
static int filter_handler(cntxt * restrict context, const char * restrict value,
    const char ** restrict err);

//  version : Affiche sur la sortie la date de compilation du programme suivit
//    du compilateur utiliser avec sa version si et seulement si le programme
//    a été compiler soit avec GCC ou CLANG. Sinon affiche Unknow Compiler.
//    Retourne une valeur nul.
static int version(void *cntxt, const char *value, const char **err);

//--- Utilitaire ---------------------------------------------------------------

//  str_hashfun : l'une des fonctions de pré-hachage conseillées par Kernighan
//    et Pike pour les chaines de caractères.
static size_t str_hashfun(const char *d);

//  fnlines : lis sur le flot associé à f, tous les charactères d'une ligne.
//    Si context->transform est différent de NULL alors applique cette fonction
//    à tous les charactères lu, sinon aucune transformation n'est effectuer sur
//    les charactères lu. Si context->filter est différent de NULL et que
//    sa condition est vérifié alors, tentes de les ajouter à t, sinon tente de
//    les ajoutes à t. Si a la fin de la lecture, si aucune erreur n'a été
//    détécter, tente d'ajouté '\0' à t.
//    Retourne une valeur nulle en cas de succée, sinon une valeur strictement
//    positif en cas d'erreur sur la lecture du ficiher. Enfin une valeur
//    strictement négatif est renvoyé en cas d'erreur d'allocation.
static int fnlines(FILE * restrict f, da * restrict t,
    cntxt * restrict context);

//  rfree, rda_dispose : Libère les ressources alloué dynamiquement pointé par p
//    et respectivement d, puis renvoi 0.
static int rfree(void *p);
static int rda_dispose(da *d);

//  scptr_display : Sans effet si il n'y a qu'un seul fichier dans
//    context->filesptr et que cptr a une longeur de 1 ou qu'il y a plusieur
//    fichier et que la longeur de cptr est différente du nombre de fichier.
//    Sinon affiche sur la sortie standard le contenu de cptr avec comme
//    séparateur le charactère ',' si il n'y a qu'un seul fichier sinon une
//    tabulation, puis affiche une tabulation et la chaine s.
//    Renvoie zéro en cas de succès, une valeur non nulle en cas d'échec.
static int scptr_display(cntxt * restrict context, const char * restrict s,
    da * restrict cptr);

//  aopt_once_null : Retourne une valeur non nulle si le tableau aopt de longeur
//    nmemb ne contient aucun élément NULL. Sinon renvoie une valeur nulle.
static int aopt_once_null(optparam **aopt, size_t nmemb);

//  aopt_dispose : Libère tout les nmemb éléments du tableau aopt, puis met la
//    valeur de ces champs à NULL.
static void aopt_dispose(optparam **aopt, size_t nmemb);

//  bst_search_aux : Recherche dans l'arbre binaire de recherche contenant des
//    hcell associé à t la référence d'un objet égal à celle de référence ref au
//    sens de la fonction de comparaison. Renvoie NULL si la recherche est
//    négative, la référence de sont attribut count trouvée sinon.
static da *bst_search_aux(bst *t, const char *ref);

//  hcell_cmp : Fonction de comparasaison entre 2 objets hcell.
//    Renvois un entier nul si les 2 objets sont égaux, un entier strictement
//    négatif si h1 est plus petit que h2 enfin, un entier strictement prositif
//    si h1 est plus grand que h2.
static int hcell_cmp(hcell *h1, hcell *h2);

//  Libère les ressources alloué pour la gestion de l'objet pointé par hc, puis
//    renvoi 0.
static int rhcell_dispose(hcell *hc);

//  STDIN : Valeur représentant l'entréer standard.
#define STDIN "-"

int main(int argc, char **argv) {
  int r = EXIT_SUCCESS;
  optparam *aop[] = {
    opt_init('u', "uppercasing", "Capitalize the characters read and process",
        false, false,
        (int (*)(void *, const char *, const char **))uppercasing_handler),
    opt_init('N', "no-color", "To disable the color on the standard output",
        false, false,
        (int (*)(void *, const char *, const char **))no_color_handler),
    opt_init('a', "avl", "To use an AVL to process the file",
        false, false,
        (int (*)(void *, const char *, const char **))avl_handler),
    opt_init('f', "filter", "To filter the characters read, according to the "
        "function pass in parameter CLASS, such as isCLASS is a function of "
        "ctype", true, false,
        (int (*)(void *, const char *, const char **))filter_handler),
    opt_init('s', "sort", "To display the results sort according to the "
        "function pass in parameter \"standard\" or \"local\", standard for "
        "the order of C and local for the order of system", true, false,
        (int (*)(void *, const char *, const char **))sort_handler),
    opt_init('v', "version", "display the version", false, true, version)
  };
  cntxt context = {
    .filesptr = da_empty(sizeof(char *)), .filter = NULL, .transform = NULL,
    .use_color = true, .use_avl = false, .sort = NULL
  };
  FILE *f = NULL;
  void *hoa = NULL;
  holdall *has = holdall_empty();
  holdall *hada = holdall_empty();
  da *line = da_empty(sizeof(char));
  if (context.filesptr == NULL || has == NULL || hada == NULL || line == NULL
      || aopt_once_null(aop, sizeof(aop) / sizeof(*aop)) != 0) {
    goto err_allocation;
  }
  optreturn ot;
  const char *err;
  if ((ot = opt_process(argc - 1, argv + 1, (const optparam **)aop,
      sizeof(aop) / sizeof(*aop),
      (int (*)(void *, const char *, const char **))file_handler, &context,
      &err, "-", "--", "If only one FILE, then returns to the standard output"
      " the numbers and contents of the equivalent lines.\nIf several FILES, "
      "then returns to the standard output, the number of occurrences of the "
      "equivalent lines present in the FILES.\n\nWithout FILE or when an "
      "argument is -, read the standard input.", "[OPTION]... [FILE]..."))
      != DONE) {
    switch(ot) {
      case STOP_PROCESS:
        goto dispose;
      case ERROR_HDL:
      case ERROR_DEFAULT:
        ERROR(context, err);
        goto error;
      case ERROR_AMB:
        ERROR2(context, "Option '", err, "' is ambigous.");
        goto error;
      case ERROR_PARAM:
        ERROR2(context, "Option '", err, "' missing argument.");
        goto error;
      default:
        ERROR2(context, "Unknow option '", err, "'.");
        goto error;
    }
  }
  if (da_length(context.filesptr) == 0) {
    const char *s = STDIN;
    if (da_add(context.filesptr, &s) == NULL) {
      goto err_allocation;
    }
  }
  if (context.use_avl) {
    hoa = bst_empty((int (*)(const void *, const void *))hcell_cmp);
  } else {
    hoa = hashtable_empty((int (*)(const void *, const void *))strcmp,
        (size_t (*)(const void *))str_hashfun);
  }
  if (hoa == NULL) {
    goto err_allocation;
  }
  for (size_t i = 0; i < da_length(context.filesptr); ++i) {
    char *filename = *(char **) da_nth(context.filesptr, i);
    if (strcmp(filename, STDIN) == 0) {
      f = stdin;
    } else {
      f = fopen(filename, "r");
      if (f == NULL) {
        goto err_open_file;
      }
    }
    long int n = 1;
    int c;
    while ((c = fnlines(f, line, &context)) == 0) {
      if (da_length(line) > 1) {
        da *cptr;
        if (context.use_avl) {
          cptr = bst_search_aux(hoa, da_nth(line, 0));
        } else {
          cptr = hashtable_search(hoa, da_nth(line, 0));
        }
        if (i == 0) {
          if (cptr != NULL) {
            if (da_length(context.filesptr) > 1) {
              *(long int *) da_nth(cptr, i) += 1;
            } else {
              if (da_add(cptr, &n) == NULL) {
                goto err_allocation;
              }
            }
          } else {
            da *dcptr = da_empty(sizeof(size_t));
            if (dcptr == NULL) {
              goto err_allocation;
            }
            char *w = malloc(da_length(line));
            if (w == NULL) {
              da_dispose(&dcptr);
              goto err_allocation;
            }
            strcpy(w, (char *) da_nth(line, 0));
            long int z = da_length(context.filesptr) > 1 ? 1 : n;
            if (context.use_avl) {
              hcell *hc = malloc(sizeof *hc);
              if (hc == NULL) {
                free(w);
                da_dispose(&dcptr);
                goto err_allocation;
              }
              hc->ref = w;
              hc->count = dcptr;
              if (da_add(dcptr, &z) == NULL
                  || holdall_put(has, w) != 0
                  || holdall_put(hada, hc) != 0
                  || bst_add_endofpath(hoa, hc) == NULL) {
                free(w);
                free(hc);
                da_dispose(&dcptr);
                goto err_allocation;
              }
            } else {
              if (da_add(dcptr, &z) == NULL
                  || holdall_put(has, w) != 0
                  || holdall_put(hada, dcptr) != 0
                  || hashtable_add(hoa, w, dcptr) == NULL) {
                free(w);
                da_dispose(&dcptr);
                goto err_allocation;
              }
            }
          }
        } else if (cptr != NULL && da_length(cptr) >= i) {
          if (da_length(cptr) == i) {
            long int z = 1;
            if (da_add(cptr, &z) == NULL) {
              goto err_allocation;
            }
          } else {
            *(long int *) da_nth(cptr, i) += 1;
          }
        }
      } else if (feof(f)) {
        da_reset(line);
        break;
      }
      ++n;
      da_reset(line);
    }
    if (f != stdin) {
      if (fclose(f) != 0) {
        f = NULL;
        goto err_file;
      }
    } else {
      clearerr(f);
    }
    f = NULL;
    if (c < 0) {
      goto err_allocation;
    } else if (c > 0) {
      goto err_file;
    }
  }
  if (context.sort != NULL) {
    holdall_sort(has, (int (*)(const void *, const void *))context.sort);
  }
  for (size_t k = 0; k < da_length(context.filesptr); ++k) {
    printf("%s", *(char **) da_nth(context.filesptr, k));
    if (k != da_length(context.filesptr) - 1) {
      putchar('\t');
    }
  }
  putchar('\n');
  if (holdall_apply_context2(has,
      hoa, (context.use_avl ? (void *(*)(void *, void *))bst_search_aux
      : (void *(*)(void *, void *))hashtable_search), &context,
      (int (*)(void *, void *, void *))scptr_display) != 0) {
    goto error_write;
  }
  goto dispose;
error_write:
  ERROR(context, "A write error occurs");
  goto error;
err_open_file:
  ERROR(context, "file doesn't exist");
  goto error;
err_file:
  ERROR(context, "on file");
  goto error;
err_allocation:
  ERROR(context, "not enougth memory");
error:
  r = EXIT_FAILURE;
dispose:
  if (f != NULL && f != stdin) {
    fclose(f);
  }
  da_dispose(&context.filesptr);
  if (context.use_avl) {
    bst_dispose((bst **) &hoa);
  } else {
    hashtable_dispose((hashtable **) &hoa);
  }
  if (has != NULL) {
    holdall_apply(has, (int (*)(void *))rfree);
  }
  holdall_dispose(&has);
  if (hada != NULL) {
    holdall_apply(hada, (context.use_avl ? (int (*)(void *))rhcell_dispose
        : (int (*)(void *))rda_dispose));
  }
  holdall_dispose(&hada);
  da_dispose(&line);
  aopt_dispose(aop, sizeof(aop) / sizeof(*aop));
  return r;
}

size_t str_hashfun(const char *s) {
  size_t h = 0;
  for (const unsigned char *p = (const unsigned char *) s; *p != '\0'; ++p) {
    h = 37 * h + *p;
  }
  return h;
}

int file_handler(cntxt *context, const char *filename, const char **err) {
  if (da_add(context->filesptr, &filename) == NULL) {
    *err = "Not enough memory";
    return -1;
  }
  return 0;
}

int fnlines(FILE *f, da *t, cntxt *context) {
  int c;
  while ((c = fgetc(f)) != EOF && c != '\n') {
    c = context->transform == NULL ? c : context->transform(c);
    if ((context->filter == NULL || context->filter(c))
        && da_add(t, &c) == NULL) {
      return -1;
    }
  }
  if (ferror(f) != 0) {
    return 1;
  }
  c = '\0';
  if (da_add(t, &c) == NULL) {
    return -1;
  }
  return 0;
}

int scptr_display(cntxt *context, const char *s, da *cptr) {
  if (da_length(context->filesptr) == 1 ? da_length(cptr) == 1
      : da_length(cptr) != da_length(context->filesptr)) {
    return 0;
  }
  int r = 0;
  for (size_t k = 0; k < da_length(cptr) - 1; ++k) {
    r = printf("%d%c", *(int *) da_nth(cptr, k),
        da_length(context->filesptr) == 1 ? ',' : '\t') < 0 ? -1 : r;
  }
  r = printf("%d\t%s\n", *(int *) da_nth(cptr, da_length(cptr) - 1), s) < 0
      ? -1 : r;
  return r;
}

int rfree(void *p) {
  free(p);
  return 0;
}

int rda_dispose(da *d) {
  da_dispose(&d);
  return 0;
}

int sort_handler(cntxt *context, const char *value,
    const char **err) {
  if (strcmp(value, STD) == 0) {
    context->sort = (int (*)(const void *, const void *))strcmp;
  } else if (strcmp(value, LC) == 0) {
    setlocale(LC_COLLATE, "");
    context->sort = (int (*)(const void *, const void *))strcoll;
  } else {
    *err = "Invalid parameter";
    return -1;
  }
  *err = NULL;
  return 0;
}

int aopt_once_null(optparam **aopt, size_t nmemb) {
  for (size_t k = 0; k < nmemb; ++k) {
    if (aopt[k] == NULL) {
      return -1;
    }
  }
  return 0;
}

void aopt_dispose(optparam **aopt, size_t nmemb) {
  for (size_t k = 0; k < nmemb; ++k) {
    opt_dispose(&(aopt[k]));
  }
}

int hcell_cmp(hcell *h1, hcell *h2) {
  return strcmp(h1->ref, h2->ref);
}

da *bst_search_aux(bst *t, const char *ref) {
  hcell hc;
  hc.ref = ref;
  hcell *r = bst_search(t, &hc);
  return r == NULL ? NULL : r->count;
}

int rhcell_dispose(hcell *hc) {
  if (hc != NULL) {
    da_dispose(&(hc->count));
    free(hc);
  }
  return 0;
}

int version(__attribute__((unused)) void *cntxt, __attribute__(
      (unused))const char *value, __attribute__((unused))const char **err) {
  printf("build on %s with ", __DATE__);
#ifdef __clang__
  printf("clang %d.%d.%d\n", __clang_major__, __clang_minor__,
      __clang_patchlevel__);
#elif defined(__GNUC__)
  printf("gcc %d.%d.%d\n", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#else
  printf("Unknow Compiler\n");
#endif
  return 0;
}

int filter_handler(cntxt *context, const char *value, const char **err) {
  struct category cl[] = {
    {ALNUM, isalnum},
    {ALPHA, isalpha},
    {BLANK, isblank},
    {DIGIT, isdigit},
    {UPPER, isupper},
    {LOWER, islower},
    {SPACE, isspace},
    {CNTRL, iscntrl},
    {GRAPH, isgraph},
    {PRINT, isprint},
    {PUNCT, ispunct},
    {XDIGIT, isxdigit}
  };
  for (size_t k = 0; k < sizeof(cl) / sizeof(*cl); ++k) {
    if (strcmp(cl[k].label, value) == 0) {
      context->filter = cl[k].cond;
      *err = NULL;
      return 0;
    }
  }
  *err = "Invalid parameter";
  return -1;
}

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <locale.h>
#include "da.h"
#include "holdall.h"
#include "hashtable.h"
#include "opt.h"

//--- Macro définissant les couleurs dans le terminal -------------------------

#define ANSI_RED    "\033[0;31m"
#define ANSI_GREEN  "\033[0;32m"
#define ANSI_YELLOW "\033[0;33m"
#define ANSI_NORM   "\033[0;0m"

//--- Macro d'affichage d'erreur -----------------------------------------------

#define MESSAGE_GEN(color, type_msg, msg, useColor)                            \
  fprintf(stderr, "%s" "*** "type_msg ": %s" "%s" "\n", useColor ? color : "", \
    msg, useColor ? ANSI_NORM : "")

#define ERROR(context, err) MESSAGE_GEN(ANSI_RED, "Error", err,                \
    (context).use_color)

#define SYNTAX(context, err) MESSAGE_GEN(ANSI_YELLOW, "Syntax", err,           \
    (context).use_color)

//--- Macro des options --------------------------------------------------------

#define AOPT_LENGTH 4
#define AOPT opt_gen("-f", "--filter", "la fonction de filtre", true,          \
    (int (*)(void *, const char *, const char **))filter),                     \
  opt_gen("-u", "--uppercasing", "met en majuscule", false,                    \
    (int (*)(void *, const char *, const char **))handle_uppercasing),         \
  opt_gen("-nc", "--no-color", "enlève les couleurs", false,                   \
    (int (*)(void *, const char *, const char **))handle_no_color),            \
  opt_gen("-s", "--sort", "trie la chienté", true,                             \
    (int (*)(void *, const char *, const char **))sort_handler)                \

#define DESC                                                                   \
  "Le projet consiste à écrire un programme en C dont le but est :\n"          \
  "— si le nom d’un seul fichier figure sur la ligne de commande,"             \
  " d’afficher, pour chaque ligne de texte non vide apparaissant au moins "    \
  "deux fois dans le fichier, les numéros des lignes où elle apparait et le"   \
  "contenu de la ligne ;\n — si au moins deux noms de fichiers figurent sur "  \
  "la ligne de commande, d’afficher, pour chaque ligne de texte non vide "     \
  "apparaissant au moins une fois dans tous les fichiers, le nombre "          \
  "d’occurrences de la ligne dans chacun des fichiers et le contenu de la "    \
  "ligne.\n L’affichage se fait en colonnes sur la sortie standard. Les "      \
  "colonnes sont (uniquement) séparées"                                        \

//--- Structure de context -----------------------------------------------------

typedef struct {
  int (*filter)(int c);
  int (*class)(int c);
  bool use_color;
  int (*sort)(const void *, const void *);
  da *filesptr;
} cntxt;

//--- Fonction de traitement d'option ------------------------------------------

#define HANDLE_PARAM_NO_ARG(fun, attribut, value)                              \
  static int handle_ ## fun(cntxt * context, __attribute__((unused))           \
    const char *res, const char **err) {                                       \
    *err = NULL;                                                               \
    context->attribut = value;                                                 \
    return 0;                                                                  \
  }

HANDLE_PARAM_NO_ARG(uppercasing, class, toupper)
HANDLE_PARAM_NO_ARG(no_color, use_color, false)

static int filter(cntxt *context, const char *value, const char **err);

//  file_handler : Ajoute le nom du fichier pointer par filename, au champs
//    filesptr du context.
//    Retourne une valeur null en cas d'echecs, et met *err à la valeur NULL.
//    Sinon renvoie une valeur non nul, et le chaine pointer par *err représente
//    l'erreur.
static int file_handler(cntxt * restrict context,
    const char * restrict filename, const char **err);

static int sort_handler(cntxt * restrict context,
    const char * restrict filename, const char **err);

//--- Utilitaire ---------------------------------------------------------------

//  str_hashfun : l'une des fonctions de pré-hachage conseillées par Kernighan
//    et Pike pour les chaines de caractères.
static size_t str_hashfun(const char *d);

//  fnlines : lis tous les charactères d'une ligne sur le flot associé à f. Pour
//    tous les charactres lu transformer par la fonction context->class, si
//    c'est charactère vérifient la condition context->filter, tentes de les
//    ajouter à t. A la fin de la lecture, si aucune erreur n'a été détécter,
//    tente d'ajouté '\0' à t.
//    Retourne une valeur nul en cas de succé, sinon une valeur strictement
//    positif en cas d'erreur sur la lecture du ficiher. Enfin une valeur
//    strictement négatif en cas d'erreur d'allocation.
static int fnlines(FILE *f, da *t, cntxt *context);

static int rfree(void *p);

static int rda_dispose(da *d);

static int scptr_display(cntxt *context, const char *s, da *cptr);

//  is_zero: retourne vrai si c est egale a 0
static bool is_zero(int *c);

int main(int argc, char **argv) {
  /*A revoir car pas de demande de couleur pour cette erreur*/
  int r = EXIT_SUCCESS;
  optparam *aop[] = {
    AOPT
  };
  cntxt context = {
    .filesptr = da_empty(sizeof(char *)), .filter = NULL, .class = NULL,
    .use_color = true, .sort = NULL
  };
  FILE *f = NULL;
  hashtable *ht = hashtable_empty((int (*)(const void *, const void *))strcmp,
      (size_t (*)(const void *))str_hashfun);
  holdall *has = holdall_empty();
  holdall *hada = holdall_empty();
  da *line = da_empty(sizeof(char));
  if (context.filesptr == NULL || ht == NULL || has == NULL || hada == NULL
      || line == NULL) {
    goto err_allocation;
  }
  optreturn ot;
  const char *err;
  if ((ot = opt_init(argv, argc, aop, AOPT_LENGTH,
      (int (*)(void *, const char *, const char **))file_handler, &context,
      &err, "[OPTION]... <file>", DESC)) != DONE) {
    if (ot == STOP_PROCESS) {
      goto dispose;
    }
    if (ot == NO_PARAM) {
      SYNTAX(context, "No paramettre where given, see the help for more"
          " information");
    } else {
      ERROR(context, err);
    }
    goto error;
  }
  if (da_length(context.filesptr) == 0) {
    goto error_no_file;
  }
  /*boucle principal*/
  for (size_t i = 0; i < da_length(context.filesptr); ++i) {
    char *filename = *(char **) da_nth(context.filesptr, i);
    if (*filename == '-' && *(filename + 1) == '\0') {
      f = stdin;
    } else {
      f = fopen(filename, "r");
    }
    if (f == NULL) {
      goto err_open_file;
    }
    long int n = 1;
    int c;
    while ((c = fnlines(f, line, &context)) == 0) {
      if (da_length(line) > 1) {
        da *cptr = hashtable_search(ht, da_nth(line, 0));
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
            if (da_length(context.filesptr) > 1) {
              /*Peut être à faire autre pars*/
              size_t k = 0;
              long int z = 1;
              while (k < da_length(context.filesptr)
                  && da_add(dcptr, &z) != NULL) {
                z = 0;
                ++k;
              }
              /* --------------------------------------------------------------
               * Même code que en dessous*/
              char *w = malloc(da_length(line));
              if (w == NULL) {
                da_dispose(&dcptr);
                goto err_allocation;
              }
              strcpy(w, (char *) da_nth(line, 0));
              if (k != da_length(context.filesptr)
                  || holdall_put(has, w) != 0
                  || holdall_put(hada, dcptr) != 0
                  || hashtable_add(ht, w, dcptr) == NULL) {
                free(w);
                da_dispose(&dcptr);
                goto err_allocation;
              }
            } else {
              char *w = malloc(da_length(line));
              if (w == NULL) {
                da_dispose(&dcptr);
                goto err_allocation;
              }
              strcpy(w, (char *) da_nth(line, 0));
              if (da_add(dcptr, &n) == NULL
                  || holdall_put(hada, dcptr) != 0
                  || holdall_put(has, w) != 0
                  || hashtable_add(ht, w, dcptr) == NULL) {
                free(w);
                da_dispose(&dcptr);
                goto err_allocation;
              }
              /*--------------------------------------------------------------*/
            }
          }
        } else if (cptr != NULL) {
          *(long int *) da_nth(cptr, i) += 1;
        }
      } else if (feof(f)) {
        break;
      }
      ++n;
      da_reset(line);
    }
    if (f != stdin && fclose(f) != 0) {
      f = NULL;
      goto err_file;
    }
    if (c < 0) {
      goto err_allocation;
    } else if (c > 0) {
      goto err_file;
    }
    f = NULL;
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
      ht, (void *(*)(void *, void *))hashtable_search,
      &context, (int (*)(void *, void *, void *))scptr_display) != 0) {
    goto error_write;
  }
  goto dispose;
error_no_file:
  ERROR(context, "Does have a file/s");
  goto error;
error_write:
  ERROR(context, "A write error occurs");
  goto error;
err_open_file:
  ERROR(context, "file desn't exist");
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
    /*Ce fclose ne s'éxecute que dans le cas où y a déjà eu une erreur donc pas
     *  besoin de le tester*/
    fclose(f);
  }
  da_dispose(&context.filesptr);
  hashtable_dispose(&ht);
  if (has != NULL) {
    holdall_apply(has, (int (*)(void *))rfree);
  }
  holdall_dispose(&has);
  if (hada != NULL) {
    holdall_apply(hada, (int (*)(void *))rda_dispose);
  }
  holdall_dispose(&hada);
  da_dispose(&line);
  return r;
}

int add_to_ptr(char *c, size_t *h) {
  *h = 37 * *h + (size_t) *c;
  return 0;
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

static int sort_handler(cntxt * restrict context, const char * restrict value,
    const char **err) {
  if (strcmp(value, "standard") == 0) {
    context->sort = (int (*)(const void *, const void *))strcmp;
  } else if (strcmp(value, "local") == 0) {
    setlocale(LC_COLLATE, "");
    context->sort = (int (*)(const void *, const void *))strcoll;
  } else {
    *err = "Pas le bon paramettre\n";
    return -1;
  }
  *err = NULL;
  return 0;
}

int rda_dispose(da *d) {
  da_dispose(&d);
  return 0;
}

int fnlines(FILE *f, da *t, cntxt *context) {
  int c;
  while ((c = fgetc(f)) != EOF && c != '\n') {
    c = context->class == NULL ? c : context->class(c);
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
  if ((da_length(context->filesptr) == 1 && da_length(cptr) == 1)
      || (da_cond_left_search(cptr, (bool (*)(const void *))is_zero) != NULL)) {
    return 0;
  }
  int r = 0;
  char separator = da_length(context->filesptr) == 1 ? ',' : '\t';
  for (size_t k = 0; k < da_length(cptr) - 1; ++k) {
    r = printf("%d%c", *(int *) da_nth(cptr, k), separator) < 0 ? -1 : r;
  }
  r = printf("%d\t%s\n", *(int *) da_nth(cptr, da_length(cptr) - 1),
      s) < 0 ? -1 : r;
  return r;
}

int rfree(void *p) {
  free(p);
  return 0;
}

bool is_zero(int *c) {
  return *c == 0;
}

int filter(cntxt *context, const char *value, const char **err) {
  if (strcmp(value, "alnum") == 0) {
    context->filter = isalnum;
  } else if (strcmp(value, "alpha") == 0) {
    context->filter = isalpha;
  } else if (strcmp(value, "blank") == 0) {
    context->filter = isblank;
  } else if (strcmp(value, "cntrl") == 0) {
    context->filter = iscntrl;
  } else if (strcmp(value, "graph") == 0) {
    context->filter = isgraph;
  } else if (strcmp(value, "lower") == 0) {
    context->filter = islower;
  } else if (strcmp(value, "print") == 0) {
    context->filter = isprint;
  } else if (strcmp(value, "punct") == 0) {
    context->filter = ispunct;
  } else if (strcmp(value, "space") == 0) {
    context->filter = isspace;
  } else if (strcmp(value, "upper") == 0) {
    context->filter = isupper;
  } else if (strcmp(value, "xdigit") == 0) {
    context->filter = isxdigit;
  } else {
    *err = "Pas le bon paramettre\n";
    return -1;
  }
  *err = NULL;
  return 0;
}

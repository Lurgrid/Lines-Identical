#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include "da.h"
#include "holdall.h"
#include "hashtable.h"
#include "opt.h"

#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define DEFAULT "\033[0;0m"

#define MESSAGE_GEN(color, type_msg, msg, useColor) \
  fprintf(stderr, "%s" "*** "type_msg ": %s" "%s" "\n", useColor ? color : "", \
    msg, useColor ? DEFAULT : "")

#define ERROR(context, err) MESSAGE_GEN(RED, "Error", err, (context).use_color)

typedef struct {
  int (*filter)(int c);
  int (*class)(int c);
  bool use_color;
  da *filesptr;
} cntxt;

//  str_hashfun : l'une des fonctions de pré-hachage conseillées par Kernighan
//    et Pike pour les chaines de caractères.
static size_t str_hashfun(const char *s);

//  file_handler : Ajoute le nom du fichier pointer par filename, au champs
//    filesptr du context.
//    Retourne une valeur null en cas d'echecs, et met *err à la valeur NULL.
//    Sinon renvoie une valeur non nul, et le chaine pointer par *err représente
//    l'erreur.
static int file_handler(cntxt * restrict context,
    const char * restrict filename, const char **err);

//  rda_dispose : libère les ressources alloué à l'utilisation de d et renvoie 0
static int rda_dispose(da *d);

//  rfree : libère la zone mémoire pointée par ptr et renvoie zéro.
static int rfree(void *ptr);

//  fnlines : lis tous les charactères d'une ligne sur le flot associé à f. Pour
//    tous les charactres lu transformer par la fonction context->class, si
//    c'est charactère vérifient la condition context->filter, tentes de les
//    ajouter à t. A la fin de la lecture, si aucune erreur n'a été détécter,
//    tente d'ajouté '\0' à t.
//    Retourne une valeur nul en cas de succé, sinon une valeur strictement
//    positif en cas d'erreur sur la lecture du ficiher. Enfin une valeur
//    strictement négatif en cas d'erreur d'allocation.
static int fnlines(FILE *f, da *t, cntxt *context);

static int scptr_display(cntxt *context, const char *s, da *cptr);

//  is_zero: retourne vrai si c est egale a 0
static bool is_zero(int *c);

//  nothing : retourne l'entier c.
static int nothing(int c);

//  rzero : retourne c - c.
static int rzero(int c);

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, YELLOW "*** Syntax: %s [OPTION]... <file>"DEFAULT "\n",
        argv[0]);
    return EXIT_FAILURE;
  }
  int r = EXIT_SUCCESS;
  cntxt context;
  FILE *f = NULL;
  context.filesptr = da_empty(sizeof(char *));
  context.filter = rzero;
  context.class = nothing;
  context.use_color = true;
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
  if ((ot
        = opt_init(argv, argc, NULL, 0,
          (int (*)(void *, const char *, const char **))file_handler, &context,
          &err, "[OPTION]... <file>",
          "Le projet consiste à écrire un programme en C dont le but est :\n"
          "— si le nom d’un seul fichier figure sur la ligne de commande,"
          " d’afficher, pour chaque ligne de texte non vide apparaissant au moins "
          "deux fois dans le fichier, les numéros des lignes où elle apparait et le"
          "contenu de la ligne ;\n — si au moins deux noms de fichiers figurent sur "
          "la ligne de commande, d’afficher, pour chaque ligne de texte non vide "
          "apparaissant au moins une fois dans tous les fichiers, le nombre "
          "d’occurrences de la ligne dans chacun des fichiers et le contenu de la "
          "ligne.\n L’affichage se fait en colonnes sur la sortie standard. Les "
          "colonnes sont (uniquement) séparées")) != DONE) {
    if (ot == STOP_PROCESS) {
      goto dispose;
    }
    ERROR(context, err);
    goto error;
  }
  if (da_length(context.filesptr) == 0) {
    goto error_no_file;
  }
  /*boucle principal*/
  for (size_t i = 0; i < da_length(context.filesptr); ++i) {
    char *filename = *(char **) (da_nth(context.filesptr, i));
    if (*filename == '-') {
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
        /*peut être a retravailler*/
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
              size_t k = 0;
              long int z = 1;
              while (k < da_length(context.filesptr)
                  && da_add(dcptr, &z) != NULL) {
                z = 0;
                ++k;
              }
              char *w = malloc(da_length(line));
              if (w == NULL) {
                da_dispose(&dcptr);
                goto err_allocation;
              }
              strcpy(w, da_nth(line, 0));
              if (k != da_length(context.filesptr)
                  || holdall_put(has, w) != 0
                  || holdall_put(hada, dcptr) != 0
                  || hashtable_add(ht, w, dcptr) == NULL) {
                da_dispose(&dcptr);
                goto err_allocation;
              }
            } else {
              char *w = malloc(strlen(da_nth(line, 0)) + 1);
              if (w == NULL) {
                da_dispose(&dcptr);
                goto err_allocation;
              }
              strcpy(w, da_nth(line, 0));
              if (da_add(dcptr, &n) == NULL
                  || holdall_put(hada, dcptr) != 0
                  || holdall_put(has, w) != 0
                  || hashtable_add(ht, w, dcptr) == NULL) {
                da_dispose(&dcptr);
                goto err_allocation;
              }
            }
          }
        } else if (cptr != NULL) {
          *(long int *) da_nth(cptr, i) += 1;
        }
      } else if (feof(f)) {
        da_reset(line);
        break;
      }
      ++n;
      da_reset(line);
    }
    if (f != stdin && fclose(f) != 0) {
      fprintf(stderr, "Nique ta mere\n");
      goto err_file;
    }
    if (c < 0) {
      goto err_allocation;
    } else if (c > 0) {
      goto err_file;
    }
    f = NULL;
  }
  for (size_t k = 0; k < da_length(context.filesptr); ++k) {
    printf("%s", * (char**) da_nth(context.filesptr, k));
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
  if (f != NULL) {
    fclose(f);
  }
  da_dispose(&context.filesptr);
  hashtable_dispose(&ht);
  if (has != NULL) {
    holdall_apply(has, rfree);
  }
  holdall_dispose(&has);
  if (hada != NULL) {
    holdall_apply(hada, (int (*)(void *))rda_dispose);
  }
  holdall_dispose(&hada);
  da_dispose(&line);
  return r;
}

size_t str_hashfun(const char *s) {
  size_t h = 0;
  for (const unsigned char *p = (const unsigned char *) s; *p != '\0'; ++p) {
    h = 37 * h + *p;
  }
  return h;
}

int file_handler(cntxt *context,
    const char *filename, const char **err) {
  if (da_add(context->filesptr, &filename) == NULL) {
    *err = "Not enough memory";
    return -1;
  }
  return 0;
}

int rda_dispose(da *d) {
  da_dispose(&d);
  return 0;
}

int rfree(void *ptr) {
  free(ptr);
  return 0;
}

int fnlines(FILE *f, da *t, cntxt *context) {
  int c;
  while ((c = fgetc(f)) != EOF && c != '\n') {
    c = context->class(c);
    if (context->filter(c) && da_add(t, &c) == NULL) {
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
  if (da_length(context->filesptr) == 1) {
    if (da_length(cptr) == 1) {
      return 0;
    }
    for (size_t k = 0; k < da_length(cptr) - 1; ++k) {
      printf("%d,", *(int *)da_nth(cptr, k));
    }
    printf("%d\t%s\n", *(int *)da_nth(cptr, da_length(cptr) - 1), s);
  } else {
    if (da_cond_left_search(cptr, (bool (*)(const void *))is_zero) == NULL) {
      for (size_t k = 0; k < da_length(cptr) - 1; ++k) {
        printf("%d\t", *(int *)da_nth(cptr, k));
      }
      printf("%d\t%s\n", *(int *)da_nth(cptr, da_length(cptr) - 1), s);
    }
  }
  return 0;
}

int nothing(int c) {
  return c;
}

int rzero(int c) {
  return c;
}

bool is_zero(int *c) {
  return *c == 0;
}

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
  int nb_file;
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
static int fnlines(FILE *f, da *t, cntxt* context);

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, YELLOW "*** Syntax: %s [OPTION]... <file>"DEFAULT "\n",
        argv[0]);
    return EXIT_FAILURE;
  }
  int r = EXIT_SUCCESS;
  cntxt context;
  context.use_color = true;
  FILE *f = NULL;
  context.filesptr = da_empty(sizeof(char *));
  hashtable *ht = hashtable_empty((int (*)(const void *, const void *))strcmp,
      (size_t (*)(const void *))str_hashfun);
  holdall *has = holdall_empty();
  holdall *hada = holdall_empty();
  da * line = da_empty(sizeof (char));
  if (context.filesptr == NULL || ht == NULL || has == NULL || hada == NULL || line == NULL) {
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
  /*boucle principal*/
  for (size_t i = 0; i < da_length(context.filesptr); ++i) {
    char *filename = *(char **)(da_nth(context.filesptr, i));
    f = fopen(filename,  "r");
    if (f == NULL) {
      fprintf(stderr, "%s\n", filename);
      goto err_open_file;
    }
    long int n = 0;
    while (
    if (fclose(f) != 0) {
      goto err_file;
    }
    f = NULL;
  }
  goto dispose;
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
  if (has != NULL) {
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

int fnlines(FILE *f, da *t, cntxt* context) {
  int c;
  while ((c = fgetc(f)) != EOF && c != '\n') {
    c = context->class(c);
    if (context->filter(c) == 0 && da_add(t, &c) == NULL) {
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

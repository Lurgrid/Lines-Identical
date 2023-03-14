#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include "da.h"
#include "holdall.h"
#include "hashtable.h"
#include "opt.h"

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

//  file_handler : Tente d'ouvrire le fichier pointé par la chaine associé à
//    filename. En cas de réussite, ajoute le pointeur associé au flot du
//    fichier, au champs filesptr de l'objet pointé par context.
//    Retourne une valeurs non nul, en cas de dépassement de capacité ou
//    d'inacesibliter au fichier pointé par filename. Sinon retourne une valeur
//    nul.
static int file_handler(cntxt * restrict context,
  const char * restrict filename);

//  fptr_close : Ferme le flot associé à **fptr.
//    Retourne une valeur non nul en cas d'echecs, sinon une valeur nul.
static int fptr_close(FILE **fptr);

//  rda_dispose : libère les ressources alloué à l'utilisation de d et renvoie 0
static int rda_dispose(da *d);

//  rfree : libère la zone mémoire pointée par ptr et renvoie zéro.
static int rfree(void *ptr);

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "*** Syntax: %s [OPTION]... <file>\n", argv[0]);
    return EXIT_FAILURE;
  }
  int r = EXIT_SUCCESS;
  cntxt context;
  context.filesptr = da_empty(sizeof (FILE *));
  hashtable *ht = hashtable_empty((int (*)(const void *, const void *))strcmp,
    (size_t (*)(const void *))str_hashfun);
  holdall *has = holdall_empty();
  holdall *hada = holdall_empty();
  if (context.filesptr == NULL || ht == NULL || has == NULL || hada == NULL) {
    goto err_allocation;
  }
  optreturn ot;
  /*
   * Changer la spé de init pour dire que toute les fonctions passer en param
   * peuvent afficher des erreures avec des printfs
   */
  if ((ot = opt_init(argv, argc, NULL, 0, (int (*) (void *, const char *)) file_handler, &context, "[OPTION]... <file>",
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
    goto error;
  }

  goto dispose;
err_allocation:
  fprintf(stderr, "y a une erreur ici\n");
error:
  r = EXIT_FAILURE;
dispose:
  if (da_apply(context.filesptr, (int (*) (void *)) fptr_close) != 0) {
    fprintf(stderr, "Erreur de fermeture\n");
    r = EXIT_FAILURE;
  }
  da_dispose(&context.filesptr);
  hashtable_dispose(&ht);
  if (has != NULL) {
    holdall_apply(has, rfree);
  }
  holdall_dispose(&has);
  if (has != NULL) {
    holdall_apply(hada, (int (*) (void *)) rda_dispose);
  }
  holdall_dispose(&hada);
  return r;
}

size_t str_hashfun(const char *s) {
  size_t h = 0;
  for (const unsigned char *p = (const unsigned char *) s; *p != '\0'; ++p) {
    h = 37 * h + *p;
  }
  return h;
}

int file_handler(cntxt * restrict context,
  const char * restrict filename) {
  FILE *f = fopen(filename, "r");
  if (f == NULL) {
    fprintf(stderr, "Erreur d'ouverture du fichier\n");
    return -1;
  }
  if (da_add(context->filesptr, &f) == NULL) {
    fprintf(stderr, "Erreur d'allocation\n");
    return -1;
  }
  return 0;
}

int fptr_close(FILE **fptr) {
  return fclose(*fptr);
}

int rda_dispose(da *d) {
  da_dispose(&d);
  return 0;
}

int rfree(void *ptr) {
  free(ptr);
  return 0;
}

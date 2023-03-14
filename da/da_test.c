#include <stdio.h>
#include <stdlib.h>
#include "da.h"

/*dans fnlines problèmes pour la spécification de la ligne vide ligne 23 à 25
 * mais aussi pour sa valeur de retour.*/

//  fnlines : Lis sur le flot pointé par f, une ligne entière et pour chaque
//    charactère lu (sauf le charactère de fin de ligne '\n' et/ou le caractère
//    de fin de fichier 'EOF'), l'ajoute au tableau dynamique associé à t, Enfin
//    après avoir ajouter touts les catactères, si il ont été ajouté alors
//    ajoute le caractère de fin de chaine de charactère '\0'.
//    Retourne une valeur nul en cas de succés, une valeur strictement négatif
//    en cas de dépacement de capacité, sinon renvoie une valeur strictement
//    positif en cas d'erreur de lecture sur le flot associé à t.
int fnlines(da *t, FILE *f) {
  int c;
  while ((c = fgetc(f)) != EOF && c != '\n') {
    if (da_add(t, &c) == NULL) {
      return -1;
    }
  }
  if (da_length(t) == 0) {
    return 0;
  }
  c = '\0';
  if (da_add(t, &c) == NULL) {
    return -1;
  }
  if (ferror(f) != 0) {
    return 1;
  }
  return 0;
}

int main(void) {
  int r = EXIT_SUCCESS;
  da *t = da_empty(sizeof(char));
  FILE *f = fopen("te.txt", "r");
  if (t == NULL) {
    goto err_alloc;
  }
  if (f == NULL) {
    goto err_file;
  }
  //  Remplacer EOF, '\n' par '\0' ATTENTION au cas ou \nEOF on ne veut pas
  // cette chaine
  int res = fnlines(t, f);
  if (res != 0) {
    if (res > 0) {
      goto err_read_line;
    } else {
      goto err_alloc;
    }
  }
  for (size_t i = 0; i < da_length(t); ++i) {
    char * cp = da_nth(t, i);
    if (cp == NULL) {
      break;
    }
    putc(*cp, stderr);
  }
  goto dispose;
err_alloc:
  fprintf(stderr, "Erreur d'allocation\n");
  r = EXIT_FAILURE;
  goto dispose;
err_read_line:
  fprintf(stderr, "Erreur de l'ecture de ligne\n");
  r = EXIT_FAILURE;
  goto dispose;
err_file:
  fprintf(stderr, "Erreur de l'ecture de fichier\n");
  r = EXIT_FAILURE;
dispose:
  if (f != NULL) {
    fclose(f);
  }
  da_dispose(&t);
  return r;
}

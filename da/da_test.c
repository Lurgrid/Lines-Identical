#include <stdio.h>
#include <stdlib.h>
#include "da.h"

int fnlines(da *t, FILE *f) {
  int c;
  while ((c = fgetc(f)) != EOF && c != '\n') {
    if (da_add(t, &c) == NULL) {
      return -1;
    }
  }
  if (da_length(t) == 0) {
    return 1;
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

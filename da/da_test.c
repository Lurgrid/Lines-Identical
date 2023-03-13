#include <stdio.h>
#include <stdlib.h>
#include "da.h"

int main(void) {
  int r = EXIT_SUCCESS;
  da *t = da_empty(sizeof(char));
  if (t == NULL) {
    return EXIT_FAILURE;
  }
  FILE *f = fopen("text_test.txt", "r");
  if (f == NULL) {
    goto err_file;
  }
  int c;
  while ((c = fgetc(f)) != EOF && c != '\n') {
    if (da_add(t, &c) == NULL) {
      goto err_read_line;
    }
  }
  //for (size_t i = 0; i < da_length(t); ++i) {
    //printf("%c", *((char *)da_nth(t, i)));
  //}
  //printf("%c", *((char *)da_nth(t, 15)));
  printf("tess");
  goto dispose;
err_read_line:
  fprintf(stderr, "Erreur de l'ecture de ligne\n");
  r = EXIT_FAILURE;
  goto dispose;
err_file:
  fprintf(stderr, "Erreur de l'ecture de fichier\n");
  r = EXIT_FAILURE;
dispose:
  fclose(f);
  da_dispose(&t);
  return r;
}


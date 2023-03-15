#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "opt.h"

int insulte(void *cntxt, const char *value);
int tmp(void *cntxt, const char *value);
void other(void *cntxt, const char *value);
int rdm(void *cntxt, const char *value);


int main(int argc, char **argv) {
  optparam *tab[] = {
    opt_gen("-t", "--tmp", "Écoute même moi je sais pas", true, tmp),
    opt_gen("-i", "--insulte", "Insulte des personne passer en paramètre", true,
        insulte),
    opt_gen("-r", "--random", "Dit une phrase randim.", false,
        rdm)
  };
  int c;
  if ((c = opt_init(argv + 1, argc - 1, tab, 3, other, NULL)) != DONE) {
    fprintf(stderr, "*** Error: ");
    switch (c) {
      case STOP_PROCESS:
        printf("STOP_PROCESS\n");
        break;
      case ERROR_FUN:
        printf("ERROR_FUN\n");
        break;
      case ERROR_PARAM:
        printf("ERROR_PARAM\n");
        break;
    }
  }
  return EXIT_SUCCESS;
}

int tmp(void *cntxt, const char *value) {
  cntxt = cntxt;
  printf("%s\n", value);
  return 0;
}

int insulte(void *cntxt, const char *value) {
  cntxt = cntxt;
  printf("%s la salope\n", value);
  return 0;
}

int rdm(void *cntxt, const char *value) {
  cntxt = cntxt;
  value = value;
  printf("pas trop random\n");
  return 0;
}

void other(void *cntxt, const char *value) {
  cntxt = cntxt;
  printf("--%s\n", value);
}

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "opt.h"

void help();
int tmp(void *cntxt, const char *value);
void  other(void *cntxt, const char *value);

int main(int argc, char **argv) {
  optparam *tab[] = {
    opt_gen("-t", "--tmp", "Voila pd", true, tmp)
  };
  int c;
  if ((c = opt_init(argv + 1, argc - 1, tab, 1, help, other, NULL)) != DONE) {
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

void help() {
  fprintf(stderr, "Nique ta mere\n");
}

int tmp(void *cntxt, const char *value) {
  cntxt = cntxt;
  printf("%s\n", value);
  return 0;
}

void other(void *cntxt, const char *value) {
  cntxt = cntxt;
  printf("--%s\n", value);
}

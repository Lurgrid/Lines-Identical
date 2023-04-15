#include <stdlib.h>
#include <stdio.h>
#include "optl.h"

static int help(void *cntxt, const char *value, const char **err);
static int lol(void *cntxt, const char *value, const char **err);
static int val(void *cntxt, const char *value, const char **err);

static int test_aopt(const optparam **aopt, size_t nmemb);

int main(int argc, char **argv) {
  const optparam *aopt[] = {
    opt_init('h', "help", "Affiche l'aide", false, true, help),
    opt_init('l', "lol", "Affiche lol", false, false, lol),
    opt_init('v', "value", "affiche la valeur", true, false, val)
  };
  if (!test_aopt(aopt, 3)) {
    fprintf(stderr, "Error : aopt is null\n");
    return EXIT_FAILURE;
  }
  const char *err;
  opt_process(argc - 1, argv + 1, aopt, sizeof(aopt) / sizeof(*aopt), NULL, NULL, &err, "-", "--");

  return EXIT_SUCCESS;
}

int help(__attribute__((unused))void *cntxt, const char *value, __attribute__((unused))const char **err) {
  return printf("Bonjour voici un help et toi ? %s\n", value) > 1;
}

int lol(__attribute__((unused))void *cntxt, __attribute__((unused))const char *value, __attribute__((unused))const char **err) {
  return printf("lol\n") == 0;
}

int val(__attribute__((unused))void *cntxt, const char *value, __attribute__((unused))const char **err) {
  return printf("Value: %s\n", value) > 1;
}

int test_aopt(const optparam **aopt, size_t nmemb) {
  for (size_t k = 0; k < nmemb; ++k) {
    if (aopt[k] == NULL) {
      return 0;
    }
  }
  return 1;
}

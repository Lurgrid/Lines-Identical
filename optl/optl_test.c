#include <stdlib.h>
#include <stdio.h>
#include "optl.h"

static int lol(void *cntxt, const char *value, const char **err);
static int val(void *cntxt, const char *value, const char **err);
static int version(void *cntxt, const char *value, const char **err);

static int test_aopt(const optparam **aopt, size_t nmemb);

int main(int argc, char **argv) {
  const optparam *aopt[] = {
    opt_init('l', "lol", "Affiche lol", false, false, lol),
    opt_init('\0', "value", "affiche la valeur", true, false, val),
    opt_init('V', "helpa", "affiche la version", false, true, version)
  };
  if (!test_aopt(aopt, sizeof(aopt) / sizeof(*aopt))) {
    fprintf(stderr, "Error : aopt is null\n");
    return EXIT_FAILURE;
  }
  const char *err;
  printf("%d\n", opt_process(argc - 1, argv + 1, aopt, sizeof(aopt) / sizeof(*aopt), NULL, NULL, &err, "-", "--", "nique ta mere", "Je viole ta mere"));
  return EXIT_SUCCESS;
}

int lol(__attribute__((unused))void *cntxt, __attribute__((unused))const char *value, __attribute__((unused))const char **err) {
  return printf("lol\n") == 0;
}

int version(__attribute__((unused))void *cntxt, __attribute__((unused))const char *value, __attribute__((unused))const char **err) {
  printf("build on %s with ", __DATE__);
  #ifdef __clang__
    printf("clang %d.%d.%d\n", __clang_major__, __clang_minor__,
        __clang_patchlevel__);
  #elif defined(__GNUC__)
    printf("gcc %d.%d.%d\n", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
  #else
    printf("Unknow Compiler\n");
  #endif
  return 0;
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

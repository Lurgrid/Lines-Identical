//
#include <stdlib.h>
#include <stdio.h>
#include "optl.h"

//  test_aopt : Renvoie 0 si tous les nmemb option du tableau aopt ne valent
//    pas NULL, sinon renvoie 1.
static int test_aopt(const optparam **aopt, size_t nmemb);

//  version : Affiche la date de compilation de l'executable et la version du
//    compilateur utiliser et renvoie 0.
static int version(void *cntxt, const char *value, const char **err);

//  test : Affiche sur la sortie standart "Value: ", suivit par la chaine de
//    caractère value et d'un retours à la ligne et renvoie 0.
static int test(void *cntxt, const char *value, const char **err);

int main(int argc, char **argv) {
  const optparam *aopt[] = {
    opt_init('V', "version", "affiche la version", false, true, version),
    opt_init('e', "keske", "test1", true, false, test),
    opt_init('f', "banon", "test2", true, false, test),
    opt_init('w', "he", "test4", false, false, test),
  };
  if (test_aopt(aopt, sizeof(aopt) / sizeof(*aopt)) != 0) {
    fprintf(stderr, "Error : Not enougth memory\n");
    return EXIT_FAILURE;
  }
  const char *err;
  optreturn r = opt_process(argc - 1, argv + 1, aopt,
      sizeof(aopt) / sizeof(*aopt), NULL, NULL, &err, "-", "--",
      "./test [-a] [-b param] [-ab] [--aaaa] [--bbbb=param]\n",
      "Programme de test pour la gestion des options par le module optl.\n");
  switch (r) {
    case ERROR_HDL:
      fprintf(stderr,
          "L'option %s à eu une erreur dans la fonction de traitement\n",
          err);
      return EXIT_FAILURE;
    case ERROR_AMB:
      fprintf(stderr, "L'option %s, est ambigue\n", err);
      return EXIT_FAILURE;
    case ERROR_PARAM:
      fprintf(stderr, "L'option %s, n'a pas reçu de paramettre\n", err);
      return EXIT_FAILURE;
    case ERROR_UNKNOWN:
      fprintf(stderr,
          "L'option %s, n'est pas prise en charge pas le programme\n", err);
      return EXIT_FAILURE;
    case ERROR_DEFAULT:
      printf("Une erreur à eu lieu lors de l'execution du traitement de ce qui"
          " n'était pas une option. \n");
      break;
    case STOP_PROCESS:
      printf("Le programme a été intérompue du à l'utilisation d'une option qui"
          " stipuler de le terminer.\n");
      break;
    case DONE:
      printf("Tous c'est bien passer.\n");
      break;
  }
  return EXIT_SUCCESS;
}

int test_aopt(const optparam **aopt, size_t nmemb) {
  for (size_t k = 0; k < nmemb; ++k) {
    if (aopt[k] == NULL) {
      return -1;
    }
  }
  return 0;
}

int version(__attribute__((unused)) void *cntxt, __attribute__(
      (unused))const char *value, __attribute__((unused))const char **err) {
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

static int test(__attribute__(
      (unused)) void *cntxt, const char *value, __attribute__(
      (unused)) const char **err) {
  printf("Value: %s\n", value);
  return 0;
}

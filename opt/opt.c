#include "opt.h"
#include <string.h>
#include <stdio.h>

#define SHORT_HELP "-h"
#define LONG_HELP "--help"

struct optparam {
  const char *optshort;
  const char *optlong;
  const char *desc;
  bool arg;
  int (*fun)(void *cntxt, const char *value);
};

optparam *opt_gen(const char *optshort, const char *optlong, const char *desc,
    bool arg, int (*fun)(void *cntxt, const char *value)) {
  optparam *op = malloc(sizeof *op);
  if (op == NULL) {
    return NULL;
  }
  op->optshort = optshort;
  op->optlong = optlong;
  op->desc = desc;
  op->arg = arg;
  op->fun = fun;
  return op;
}

//  prefix : Renvoie le premier caractère de s2 qui
static const char *prefix(const char *s1, const char *s2) {
  if (*s1 == '\0') {
    return s2;
  }
  if (*s2 == '\0' || *s1 != *s2) {
    return NULL;
  }
  return prefix(s1 + 1, s2 + 1);
}

static const char *opt_parse(const char *optlong, const char *optshort, bool arg, int *k,
    char **argv, int argc) {
  if (strcmp(optshort, argv[*k]) == 0) {
    if (arg) {
      if (*k + 1 >= argc) {
        return NULL;
      }
      const char *v = argv[*k + 1];
      ++(*k);
      return v;
    }
    return argv[*k];
  }
  const char *p = prefix(optlong, argv[*k]);
  if (p == NULL) {
    return NULL;
  }
  if (*p != '=') {
    return argv[*k];
  }
  return p + 1;
}

optreturn opt_init(char **argv, int argc, optparam **aopt, size_t nmemb,
    void (*help)(void), void (*other)(void *cntxt, const char *value),
    void *cntxt) {
  for (int k = 0; k < argc; ++k) {
    if (strcmp(SHORT_HELP, argv[k]) == 0 || strcmp(LONG_HELP, argv[k]) == 0) {
      help();
      return STOP_PROCESS;
    }
    size_t i = 0;
    char *a = argv[k];
    while (i < nmemb) {
      const char *v = opt_parse(aopt[i]->optlong, aopt[i]->optshort,
          aopt[i]->arg, &k, argv, argc);
      if (v == a) {
        return ERROR_PARAM;
      }
      if (v != NULL) {
        if (aopt[i]->fun(cntxt, v) != 0) {
          return ERROR_FUN;
        }
        break;
      }
      ++i;
    }
    if (i == nmemb) {
      other(cntxt, a);
    }
  }
  return DONE;
}

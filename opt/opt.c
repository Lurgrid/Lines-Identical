#include "opt.h"
#include <string.h>
#include <stdio.h>

#define SHORT_HELP "-h"
#define LONG_HELP "--help"

#define LONG(opt) ((opt)->optlong)
#define SHORT(opt) ((opt)->optshort)
#define ARG(opt) ((opt)->arg)

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

static int opt_parse(const char *optlong, const char *optshort, bool arg,
    int *k,
    char **argv, int argc, const char **option) {
  if (strcmp(optshort, argv[*k]) == 0) {
    if (arg) {
      if (*k + 1 >= argc) {
        *option = argv[*k];
        return -1;
      }
      ++(*k);
    }
    *option = argv[*k];
    return 0;
  }
  if (arg) {
    const char *p = prefix(optlong, argv[*k]);
    if (p == NULL) {
      *option = NULL;
      return -1;
    }
    if (*p != '=') {
      *option = argv[*k];
      return -1;
    }
    *option = p + 1;
    return 0;
  }
  if (strcmp(optlong, argv[*k]) == 0) {
    *option = argv[*k];
    return 0;
  }
  *option = NULL;
  return -1;
}

#define PRINT_OPTION(opt) printf("\t%s%s | %s%s : %s\n", opt->optshort,        \
    (opt->arg ? " [option]" : ""), opt->optlong,                               \
    (opt->arg ? "=[option]" : ""), opt->desc)                                  \

optreturn opt_init(char **argv, int argc, optparam **aopt,
    size_t nmemb, void (*other)(void *cntxt, const char *value), void *cntxt) {
  for (int k = 0; k < argc; ++k) {
    if (strcmp(SHORT_HELP, argv[k]) == 0 || strcmp(LONG_HELP, argv[k]) == 0) {
      for (size_t i = 0; i < nmemb; ++i) {
        PRINT_OPTION(aopt[i]);
      }
      return STOP_PROCESS;
    }
    size_t i = 0;
    while (i < nmemb) {
      const char *v;
      int r = opt_parse(LONG(aopt[i]), SHORT(aopt[i]), ARG(aopt[i]), &k, argv,
          argc, &v);
      if (v != NULL) {
        if (r != 0) {
          return ERROR_PARAM;
        }
        if (aopt[i]->fun(cntxt, v) != 0) {
          return ERROR_FUN;
        }
        break;
      }
      ++i;
    }
    if (i == nmemb) {
      other(cntxt, argv[k]);
    }
  }
  return DONE;
}

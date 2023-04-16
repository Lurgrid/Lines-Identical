// optgnu.c : partie implantation de l'interface optg.h

#include "optl.h"

#include <string.h>
#include <stdio.h>

//--- Définition optparam ------------------------------------------------------

//  struct optparam, optparam : representation une option d'un programme.
//    optshort, optlong représente tout deux la possible représentation de
//    l'options sous forme respectivment 'courte' ou 'long'. Une au moins est
//    obligatoire. La non prise en charge d'une options courte se spécifie par
//    le passage du charactère '\0' pour cette arguement de même pour l'option
//    longue avec la valeur NULL. Desc, une chaine de carractère, la description
//    de l'option. Arg le booléen spécifiant le besoin ou non d'un argument pour
//    cette options. Interup un booléen spécifiant si l'option arrête le
//    traitement des autres options après son traitement. hdl, une fonctoin
//    représentent l'actions à éffectuer l'ors de l'appel de l'option, avec
//    cntxt un pointeur vers un context, value le possible paramettre de
//    l'option à traiter et err un pointeur de chaine de caractère qui change de
//    valeur en cas d'erreur.
struct optparam {
  char optshort;
  const char *optlong;
  const char *desc;
  bool arg;
  bool interup;
  int (*hdl)(void *cntxt, const char *value, const char **err);
};

//--- Divers -------------------------------------------------------------------

//  prefix : Renvoie NULL si s1 n'est pas prefix de s2, si s1 est egale a s2
//    alors renvois le pointeur sur le carractère nul de s2 sinon renvois le
//    pointeur sur le premier carractère de s2 qui n'appartient pas a s1.
static const char *prefix(const char *s1, const char *s2) {
  if (*s1 == '\0') {
    return s2;
  }
  if (*s2 == '\0' || *s1 != *s2) {
    return NULL;
  }
  return prefix(s1 + 1, s2 + 1);
}

#define SH 2

#define PRINT_HELP(aopt, nmemb, usage, desc, short_cal, long_cal)              \
  if (usage != NULL) {                                                         \
    printf("Usage: %s\n", usage);                                              \
  }                                                                            \
  if (desc != NULL) {                                                          \
    printf("%s\n", desc);                                                      \
  }                                                                            \
  for (size_t k = 0; k < nmemb; ++k) {                                         \
    if (aopt[k]->optshort == '\0') {                                           \
      printf("%*s", SH + 1 + (int) strlen(short_cal), " ");                    \
    } else {                                                                   \
      printf("%*s%s%c", SH, " ", short_cal, aopt[k]->optshort);                \
    }                                                                          \
    if (aopt[k]->optlong != NULL) {                                            \
      printf("%s%s%s", (aopt[k]->optshort != '\0' ? ", " : "  "), long_cal,    \
          aopt[k]->optlong);                                                   \
      if (aopt[k]->arg) {                                                      \
        putchar(LONG_JOIN);                                                    \
        printf("[OPTION]");                                                    \
      } else {                                                                 \
        printf("\t");                                                          \
      }                                                                        \
    } else {                                                                   \
      printf("\t\t");                                                          \
    }                                                                          \
    printf("\t%s\n", aopt[k]->desc);                                           \
  }                                                                            \
  printf("%*s%s%c, %s%s\t\tdisplay this help and exit\n", SH, " ",             \
      short_cal, SHORT_HELP, long_cal, LONG_HELP);                             \

//--- Implémentation opt -------------------------------------------------------

optparam *opt_init(const char optshort, const char *optlong,
    const char *desc, bool arg, bool interup, int (*hdl)(void *cntxt,
    const char *value, const char **err)) {
  if (optshort == '\0' && optlong == NULL) {
    return NULL;
  }
  optparam *op = malloc(sizeof *op);
  if (op == NULL) {
    return NULL;
  }
  op->optshort = optshort;
  op->optlong = optlong;
  op->desc = desc;
  op->arg = arg;
  op->interup = interup;
  op->hdl = hdl;
  return op;
}

//  opt_parse_long :
static optreturn opt_parse_long(const char **param, const optparam **aopt,
    size_t nmemb, const optparam **opt) {
  if (nmemb == 0) {
    return ERROR_UNKNOWN;
  }
  register size_t min = 0;
  register size_t max = nmemb;
  register int i = 0;
  while (max > min && aopt[max - 1]->optlong == NULL) {
    --max;
  }
  if (min == max) {
    return ERROR_UNKNOWN;
  }
  while ((*param)[i] != '\0' && (*param)[i] != LONG_JOIN && min < max) {
    while (min < max && aopt[min]->optlong[i] < (*param)[i]) {
      if (aopt[min]->optlong[i] > (*param)[i]) {
        return ERROR_UNKNOWN;
      }
      ++min;
    }
    while (max > min && aopt[max - 1]->optlong[i] > (*param)[i]) {
      if (aopt[max - 1]->optlong[i] < (*param)[i]) {
        return ERROR_UNKNOWN;
      }
      --max;
    }
    if (min == max) {
      return ERROR_UNKNOWN;
    }
    ++i;
  }
  if (min + 1 == max) {
    *opt = aopt[min];
    (*param) += i;
    while (**param != '\0' && **param != LONG_JOIN) {
      ++(*param);
    }
    return DONE;
  }
  return ERROR_AMB;
}

//  opt_parse_short :
static const optparam *opt_parse_short(const char **param,
    const optparam **aopt, size_t nmemb) {
  for (size_t i = 0; i < nmemb; ++i) {
    if (aopt[i]->optshort != '\0' && **param == aopt[i]->optshort) {
      *param += 1;
      return aopt[i];
    }
  }
  return NULL;
}

static int opt_long_cmp(const optparam **opt1, const optparam **opt2) {
  if ((*opt1)->optlong == NULL) {
    return (*opt2)->optlong == NULL ? 0 : 1;
  }
  if ((*opt2)->optlong == NULL) {
    return -1;
  }
  return strcmp((*opt1)->optlong, (*opt2)->optlong);
}

optreturn opt_process(int argc, char **argv, const optparam **aopt,
    size_t nmemb, int (*hdl_dlt)(void *cntxt, const char *value,
    const char **err), void *cntxt, const char **err, const char *short_cal,
    const char *long_cal, const char *desc, const char *usage) {
  qsort(aopt, nmemb, sizeof *aopt, (int (*)(const void *, const void *))opt_long_cmp);
  char nf = 0;
  for (int i = 0; i < argc; ++i) {
    const char *endp;
    if (nf) {
      nf = 0;
      if (hdl_dlt != NULL && hdl_dlt(cntxt, argv[i], err) != 0) {
        return ERROR_DEFAULT;
      }
    } else if (strcmp(NEXT_NOPT, argv[i]) == 0) {
      nf = 1;
    } else if ((endp = prefix(long_cal, argv[i])) != NULL) {
      const optparam *opt = NULL;
      const char *fendp = endp;
      optreturn r = opt_parse_long(&endp, aopt, nmemb, &opt);
      if (r == ERROR_AMB) {
        return r;
      }
      const char *t = prefix(fendp, LONG_HELP);
      if (r == ERROR_UNKNOWN) {
        if (t != NULL) {
          PRINT_HELP(aopt, nmemb, usage, desc, short_cal, long_cal)
          *err = NULL;
          return STOP_PROCESS;
        }
        return r;
      }
      if (t != NULL) {
        if (strcmp(LONG_HELP, fendp) == 0) {
          PRINT_HELP(aopt, nmemb, usage, desc, short_cal, long_cal)
          *err = NULL;
          return STOP_PROCESS;
        }
        if (strcmp(opt->optlong, fendp) != 0) {
          *err = argv[i];
          return ERROR_AMB;
        }
      }
      if (opt->arg) {
        if (*endp != LONG_JOIN || *(endp + 1) == '\0') {
          *err = argv[i];
          return ERROR_PARAM;
        }
        if (opt->hdl(cntxt, endp + 1, err) != 0) {
          return ERROR_HDL;
        }
      } else {
        if (opt->hdl(cntxt, endp, err) != 0) {
          return ERROR_HDL;
        }
      }
      if (opt->interup) {
        *err = NULL;
        return STOP_PROCESS;
      }
    } else if ((endp = prefix(short_cal, argv[i])) != NULL && *endp != '\0') {
      const optparam *opt;
      while (*endp != '\0') {
        if (*endp == SHORT_HELP) {
          PRINT_HELP(aopt, nmemb, usage, desc, short_cal, long_cal)
          *err = NULL;
          return STOP_PROCESS;
        }
        if ((opt = opt_parse_short(&endp, aopt, nmemb)) == NULL) {
          *err = endp;
          return ERROR_UNKNOWN;
        }
        if (opt->arg) {
          if (i >= argc - 1 || *endp != '\0') {
            *err = endp;
            return ERROR_PARAM;
          }
          ++i;
        }
        if (opt->hdl(cntxt, argv[i], err) != 0) {
          return ERROR_HDL;
        }
        if (opt->interup) {
          *err = NULL;
          return STOP_PROCESS;
        }
      }
    } else {
      if (hdl_dlt != NULL && hdl_dlt(cntxt, argv[i], err) != 0) {
        return ERROR_DEFAULT;
      }
    }
  }
  *err = NULL;
  return DONE;
}

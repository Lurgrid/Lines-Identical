// optgnu.c : partie implantation de l'interface optg.h

#include "optl.h"

#include <string.h>
#include <stdio.h>

//--- Définition optparam ------------------------------------------------------

//  struct optparam, optparam : representation une option d'un programme, les
//    champs optshort, optlong représente respectivement les version courte et
//    longue de l'option. Les deux représentation ne sont pas obligatoire, si
//    l'un des deux vaut le charactère nul ('\0'), NULL respectivement alors
//    cette options est considérer sant cette représentation. desc est une
//    chaine de carractère, la description de l'option. Arg le booléen
//    spécifiant le besoin ou non d'un argument pour cette options. Interup un
//    booléen spécifiant si l'option arrête le traitement des autres options
//    après son traitement. la fonction hdl représente les actions l'ors de
//    l'appel de l'option.
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
  size_t min = 0;
  size_t max = nmemb;
  int i = 0;
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
  return nmemb == 0 ? DONE : ERROR_AMB;
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
    const char *long_cal) {
  qsort(aopt, nmemb, sizeof *aopt, (int (*)(const void *, const void *))opt_long_cmp);
  char nf = 0;
  for (int i = 0; i < argc; ++i) {
    const char *endp;
    if (nf) {
      nf = 0;
      if (hdl_dlt(cntxt, argv[i], err) != 0) {
        return ERROR_DEFAULT;
      }
    } else if (strcmp(NEXT_NOPT, argv[i]) == 0) {
      nf = 1;
    } else if ((endp = prefix(long_cal, argv[i])) != NULL) {
      const optparam *opt = NULL;
      optreturn r;
      if ((r = opt_parse_long(&endp, aopt, nmemb, &opt)) != DONE) {
        return r;
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
    } else if ((endp = prefix(short_cal, argv[i])) != NULL) {
      const optparam *opt;
      while (*endp != '\0') {
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
      if (hdl_dlt(cntxt, argv[i], err) != 0) {
        return ERROR_DEFAULT;
      }
    }
  }
  *err = NULL;
  return DONE;
}

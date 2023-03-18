// opt.c : partie implantation de l'interface opt.h

#include "opt.h"

#include <string.h>
#include <stdio.h>

//--- Définition optparam ------------------------------------------------------

//  struct optparam, optparam : representation d'une option d'un programme, les
//    champs optshort, optlong représente respectivement les version courte et
//    longue de l'option. desc est une chaine de carractère, la description de
//    l'option. le booléen vaut vrai si l'option demande un paramètre et faux
//    sinon. la fonction fun représente les actions l'ors de l'appel de l'option
//    .
struct optparam {
  const char *optshort;
  const char *optlong;
  const char *desc;
  bool arg;
  int (*fun)(void *cntxt, const char *value, const char **err);
};

//--- Racourcis ----------------------------------------------------------------

#define LONG(opt) ((opt)->optlong)
#define SHORT(opt) ((opt)->optshort)
#define ARG(opt) ((opt)->arg)

//--- Divers -------------------------------------------------------------------

//  prefix : Renvoie NULL si s1 n'est pas prefix de s2, si s1 est egale a s2
//    alors renvois le pointeur du carractère nul de s2 sinon renvois le
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

//--- OPT function -------------------------------------------------------------

optparam *opt_gen(const char *optshort, const char *optlong, const char *desc,
    bool arg, int (*fun)(void *cntxt, const char *value, const char **err)) {
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

void opt_dispose(optparam **optptr) {
  if (*optptr != NULL) {
    free(*optptr);
    *optptr = NULL;
  }
}

//  parse_return : Spécifie les valeurs de retour de la fonction opt_parse.
enum parse_return {
  SUCCESS_PARAM,
  FAILURE_PARAM,
  NOT_EQUAL,
};

//  opt_check : Vérifie si la chaine d'indice k du tableau argv (de longueur
//    argc) correspond à une entrée correct de l'option pointer par opt.
//    Retourne SUCCESS_PARAM si l'entrée était correct, option pointe alors sur
//    argv[k + 1] si le l'option a besoin d'un paramètre sinon pointe sur
//    argv[k]. Si la valuer retourner est egale a NOT_EQUAL alors l'entrée n'est
//    pas l'option pointer par opt et option vaut NULL. Enfin FAILURE_PARAM est
//    renvoyer quand l'utilisateur a rentrée l'option pointé par opt mais ne lui
//    à pas donner d'argument alors qu'elle en demander un, dans se cas option
//    pointe sur argv[k].
static enum parse_return opt_parse(const optparam *opt, int *k, char **argv, int argc,
    const char **option) {
  /*Est ce que option courte*/
  if (strcmp(SHORT(opt), argv[*k]) == 0) {
    if (ARG(opt)) {
      if (*k + 1 >= argc) {
        *option = argv[*k];
        return FAILURE_PARAM;
      }
      *k += 1;
    }
    /*si bonne option et param possible alors on augmente k et option pointe sur le param*/
    *option = argv[*k];
    return SUCCESS_PARAM;
  }
  const char *p = prefix(LONG(opt), argv[*k]);
  if (p == NULL || (!ARG(opt) && strcmp(LONG(opt), argv[*k]) == 0)) {
    *option = NULL;
    return NOT_EQUAL;
  }
  if (*p != '=') {
    *option = argv[*k];
    return FAILURE_PARAM;
  }
  *option = p + 1;
  return SUCCESS_PARAM;
}

#define PRINT_OPTION(opt) printf("\t%s%s | %s%s : %s\n", opt->optshort,        \
    (opt->arg ? " [option]" : ""), opt->optlong,                               \
    (opt->arg ? "=[option]" : ""), opt->desc)                                  \

#define SHORT_HELP "-h"
#define LONG_HELP "--help"

optreturn opt_init(char **argv, int argc, optparam **aopt,
    size_t nmemb, int (*other)(void *cntxt, const char *value,
    const char **err), void *cntxt, const char **err, const char *usage,
    const char *desc) {
  for (int k = 1; k < argc; ++k) {
    /*traitement du help*/
    if (strcmp(SHORT_HELP, argv[k]) == 0 || strcmp(LONG_HELP, argv[k]) == 0) {
      if (usage != NULL) {
        printf("Usage: %s %s\n\n", argv[0], usage);
      }
      for (size_t i = 0; i < nmemb; ++i) {
        PRINT_OPTION(aopt[i]);
      }
      if (desc != NULL) {
        printf("\n%s\n", desc);
      }
      return STOP_PROCESS;
    }
    /*C'est pas le help donc on test si c'est une option de aopt*/
    size_t i = 0;
    while (i < nmemb) {
      const char *v;
      enum parse_return r = opt_parse(aopt[i], &k, argv, argc, &v);
      if (v != NULL) {
        if (r != SUCCESS_PARAM) {
          *err = aopt[i]->optlong;
          return ERROR_PARAM;
        }
        if (aopt[i]->fun(cntxt, v, err) != 0) {
          return ERROR_FUN;
        }
        break;
      }
      ++i;
    }
    if (i == nmemb) {
      if (other(cntxt, argv[k], err) != 0) {
        return ERROR_FUN;
      }
    }
  }
  *err = NULL;
  return DONE;
}

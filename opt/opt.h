//  Partie interface du module opt (gestion d'option).
//
//  Ce module permet de mettre en place une gestion d'option pour son
//    utilisateur.

#ifndef OPT__H
#define OPT__H

#include <stdbool.h>
#include <stdlib.h>

typedef struct optparam optparam;

typedef enum {
  STOP_PROCESS,
  ERROR_FUN,
  ERROR_PARAM,
  DONE,
} optreturn;

extern optparam *opt_gen(const char *optshort, const char *optlong,
    const char *desc, bool arg, int (*fun)(void *cntxt, const char *value));

extern optreturn opt_init(char **argv, int argc, optparam **aopt,
    size_t nmemb, void (*help)(void),
    void (*other)(void *cntxt, const char *value), void *cntxt);

#endif

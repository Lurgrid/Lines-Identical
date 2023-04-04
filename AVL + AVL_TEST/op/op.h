#ifndef OP__H
#define OP__H

#include <stdlib.h>
#include <stdio.h>

#ifndef OP_NUMBER_OF_PARAMETERS_MAX
#define OP_NUMBER_OF_PARAMETERS_MAX 1
#elif OP_NUMBER_OF_PARAMETERS_MAX < 0
#error OP_NUMBER_OF_PARAMETERS_MAX must be greater than 0.
#endif

typedef struct {
  const char *lbl;
  int (*get)(size_t j, void *context);
} opparam;

typedef enum {
  OP_RETURN_SUCCESS,
  OP_RETURN_EXIT,
  OP_RETURN_HELP,
  OP_RETURN_UNKNOWN_OP,
  OP_RETURN_ABORTED_PARAMETER,
  OP_RETURN_INVALID_PARAMETER,
  OP_RETURN_CAPACITY_ERROR,
} opreturn;

typedef struct {
  int oper;
  const char *help;
#if OP_NUMBER_OF_PARAMETERS_MAX != 0
  opparam param[OP_NUMBER_OF_PARAMETERS_MAX];
#endif
  opreturn (*exec)(void *context);
} opitem;

#if OP_NUMBER_OF_PARAMETERS_MAX != 0
typedef const void *opvalue[OP_NUMBER_OF_PARAMETERS_MAX];
#endif

#define OP_ITEM_EXEC_EXIT NULL
#define OP_ITEM_EXEC_HELP op__item_exec_help

extern opreturn op__item_exec_help(void *);

extern int op_execute(opreturn (*open)(void *), void (*close)(void *),
    opitem *base, size_t nmemb,
    int tab1, int tab2, int sptab2bfr, int sptab2aftr, const char *notes[],
    void *context);

#define OP_MSG(format, ...)  \
  printf("\r--- " format "\n", __VA_ARGS__)

#define OP__TRBL(class, format, ...) \
  fprintf(stderr, "\r*** " class ": " format "\n", __VA_ARGS__)

#define OP_ERR(format, ...)  OP__TRBL("Error", format, __VA_ARGS__)
#define OP_WRN(format, ...)  OP__TRBL("Warning", format, __VA_ARGS__)

#endif

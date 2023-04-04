#include "op.h"
#include <ctype.h>
#include <sys/ioctl.h>

static const opitem *op__search(int c, const opitem *base, size_t nmemb) {
  const opitem *p = base;
  while (p < base + nmemb) {
    if (p->oper == c) {
      return p;
    }
    ++p;
  }
  return NULL;
}

#ifdef OP_CHECK

static void op__check(const opitem *base, size_t nmemb) {
  for (const opitem *p = base; p < base + nmemb; ++p) {
    if (op__search(p->oper, p + 1, nmemb - (size_t) (p + 1 - base)) != NULL) {
      fprintf(stderr,
          "Internal error: %s: duplicate operator '[%d]'\n",
          __func__, p->oper);
      abort();
    }
#if OP_NUMBER_OF_PARAMETERS_MAX != 0
    for (size_t j = 0; j < OP_NUMBER_OF_PARAMETERS_MAX; ++j) {
      if ((p->param[j].lbl == NULL) != (p->param[j].get == NULL)) {
        fprintf(stderr,
            "Internal error: %s: invalid parameter list '[%d, %zu]'\n",
            __func__, p->oper, j);
        abort();
      }
    }
#endif
  }
}

#endif

static opreturn op__call(int c, const opitem *base, size_t nmemb,
    void *context) {
  const opitem *p = op__search(c, base, nmemb);
  if (p == NULL) {
    return OP_RETURN_UNKNOWN_OP;
  }
#if OP_NUMBER_OF_PARAMETERS_MAX != 0
  size_t j = 0;
  while (j < OP_NUMBER_OF_PARAMETERS_MAX && p->param[j].get != NULL) {
    int r = p->param[j].get(j, context);
    if (r == EOF) {
      return OP_RETURN_ABORTED_PARAMETER;
    }
    if (r == 0) {
      return OP_RETURN_INVALID_PARAMETER;
    }
    ++j;
  }
#endif
  if (p->exec == OP_ITEM_EXEC_EXIT) {
    return OP_RETURN_EXIT;
  }
  if (p->exec == OP_ITEM_EXEC_HELP) {
    return OP_RETURN_HELP;
  }
  return p->exec(context);
}

#define PUTC(x, c)        { putchar(c); ++x; }
#define ADVANCE_TO(x, n)  { while (x < n) { PUTC(x, ' '); } }
#define NEW_LINE(x)       { putchar('\n'); x = 0; }

static void op__help_line(const char *s, int x, int indent, int width) {
  const char *p = s;
  while (*p != '\0') {
    const char *q = p;
    while (*q != '\0' && !isspace(*q)) {
      ++q;
    }
    if (x + q - p < width) {
      if (x > indent) {
        PUTC(x, ' ');
      }
    } else {
      NEW_LINE(x);
    }
    ADVANCE_TO(x, indent);
    while (p < q) {
      PUTC(x, *p);
      ++p;
    }
    while (isspace(*p)) {
      ++p;
    }
  }
  NEW_LINE(x);
}

static void op__help(const opitem *base, size_t nmemb,
    int tab1, int tab2, int sptab2bfr, int sptab2aftr, const char *notes[]) {
  struct winsize ws;
  if (ioctl(0, TIOCGWINSZ, &ws) == -1) {
    ws.ws_col = 80;
  }
  op__help_line("List of operations:", 0, 0, ws.ws_col);
  for (const opitem *p = base; p < base + nmemb; ++p) {
    int x = 0;
    ADVANCE_TO(x, tab1);
    PUTC(x, p->oper);
#if OP_NUMBER_OF_PARAMETERS_MAX != 0
    size_t j = 0;
    while (j < OP_NUMBER_OF_PARAMETERS_MAX && p->param[j].lbl != NULL) {
      PUTC(x, ' ');
      const char *q = p->param[j].lbl;
      while (*q != '\0') {
        PUTC(x, *q);
        ++q;
      }
      ++j;
    }
#endif
    if (x > sptab2bfr && x > tab2 - sptab2bfr) {
      if (sptab2aftr < sptab2bfr) {
        NEW_LINE(x);
      } else {
        ADVANCE_TO(x, tab2 + sptab2aftr - 1);
      }
    }
    op__help_line(p->help == NULL ? "" : p->help, x, tab2, ws.ws_col);
  }
  if (notes != NULL) {
    const char **pp = notes;
    while (*pp != NULL) {
      putchar('\n');
      op__help_line(*pp, 0, 0, ws.ws_col);
      ++pp;
    }
  }
}

opreturn op__item_exec_help(__attribute__((unused)) void *unused) {
  return OP_RETURN_HELP;
}

int op_execute(opreturn (*open)(void *), void (*close)(void *),
    opitem *base, size_t nmemb,
    int tab1, int tab2, int sptab2bfr, int sptab2aftr, const char *notes[],
    void *context) {
  //
#ifdef OP_CHECK
  op__check(base, nmemb);
#endif
  //
  int r = EXIT_SUCCESS;
  if (open(context) != OP_RETURN_SUCCESS) {
    goto error_capacity;
  }
  //
  int c = '\n';
loop:
  if (c == '\n') {
    printf("> ");
  }
  c = getchar();
  if (c == EOF) {
    goto error_interrupt;
  } else if (isspace(c)) {
    goto loop;
  }
  switch (op__call(c, base, nmemb, context)) {
    case OP_RETURN_SUCCESS:
      goto loop;
    case OP_RETURN_EXIT:
      goto end;
    case OP_RETURN_HELP:
      op__help(base, nmemb,
          tab1, tab2, sptab2bfr, sptab2aftr, notes);
      goto loop;
    case OP_RETURN_UNKNOWN_OP:
      if (isgraph(c)) {
        OP_WRN("Unknow '%c' operation", c);
      } else {
        OP_WRN("Unknow '[%d]' operation", c);
      }
      break;
    case OP_RETURN_ABORTED_PARAMETER:
      goto error_interrupt;
    case OP_RETURN_INVALID_PARAMETER:
      OP_WRN("Illegal value for operation '%c'", c);
      break;
    case OP_RETURN_CAPACITY_ERROR:
      goto error_capacity;
  }
  while ((c = getchar()) != EOF && c != '\n') {
  }
  goto loop;
  //
error_capacity:
  OP_ERR("%s", "Ask a wizard to enlarge me");
  r = EXIT_FAILURE;
  goto end;
error_interrupt:
  OP_WRN("%s", "Inelegant quit");
  goto end;
  //
end:
  close(context);
  return r;
}

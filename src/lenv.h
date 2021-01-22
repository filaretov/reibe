#ifndef LENV_H
#define LENV_H

#include "types.h"

struct lenv {
    lenv* parent;
    int count;
    char** syms;
    lval** vals;
};

lenv* lenv_new(void);
void lenv_del(lenv* e);
lval* lenv_get(lenv* e, lval* k);
lenv* lenv_copy(lenv* e);
void lenv_def(lenv* e, lval* k, lval* v);
void lenv_put(lenv* e, lval* k, lval* v);

#endif /* LENV_H */

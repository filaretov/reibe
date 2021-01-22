#ifndef STAMMER_TYPES_H
#define STAMMER_TYPES_H

struct lval;
struct lenv;
typedef struct lval lval;
typedef struct lenv lenv;

typedef lval*(*lbuiltin)(lenv*, lval*);

#endif /* STAMMER_TYPES_H */

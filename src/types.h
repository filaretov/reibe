#ifndef REIBE_TYPES_H
#define REIBE_TYPES_H

struct lval;
struct lenv;
typedef struct lval lval;
typedef struct lenv lenv;

typedef lval*(*lbuiltin)(lenv*, lval*);

#endif /* REIBE_TYPE_H */

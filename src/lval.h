#ifndef LVAL_H
#define LVAL_H

#include "types.h"

enum { LVAL_ERR, LVAL_NUM, LVAL_SYM, LVAL_STR, LVAL_SEXPR, LVAL_QEXPR, LVAL_FUN, LVAL_BOOL };
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

struct lval {
    int type;

    /* Basic */
    long num;
    char* err;
    char* sym;
    char* str;

    /* Function */
    lbuiltin builtin;
    lenv* env;
    lval* formals;
    lval* body;

    /* Expression */
    int count;
    lval** cell;
};

lval* lval_num(long x);
lval* lval_err(char* fmt, ...);
lval* lval_sym(char* s);
lval* lval_sexpr(void);
lval* lval_qexpr(void);
lval* lval_bool(int x);
lval* lval_str(char* s);
lval* lval_builtin(lbuiltin func);
lval* lval_join(lval* x, lval* y);
void lval_del(lval* v);
lval* lval_copy(lval* v);
void lval_print_str(lval* v);
void lval_print(lval* v);
void lval_println(lval* v);
lval* lval_add(lval* v, lval* x);
lval* lval_lambda(lval* formals, lval* body);
void lval_expr_print(lval* v, char open, char close);
lval* lval_call(lenv* e, lval* f, lval* a);
lval* lval_eval(lenv* e, lval* v);
lval* lval_eval_sexpr(lenv* e, lval* v);
lval* lval_pop(lval* v, int i);
lval* lval_take(lval* v, int i);
char* lval_type_name(int t);

#endif /* LVAL_H */

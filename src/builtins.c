#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "lenv.h"
#include "lval.h"
#include "builtins.h"
#include "parser.h"

void lenv_add_builtins(lenv* e) {
    /* Eval & Fn Functions */
    lenv_add_builtin(e, "eval", builtin_eval);
    lenv_add_builtin(e, "def", builtin_def);
    lenv_add_builtin(e, "=", builtin_put);
    lenv_add_builtin(e, "lambda", builtin_lambda);

    /* Math Functions */
    lenv_add_builtin(e, "+", builtin_add);
    lenv_add_builtin(e, "-", builtin_sub);
    lenv_add_builtin(e, "*", builtin_mul);
    lenv_add_builtin(e, "/", builtin_div);

    /* Lists */
    lenv_add_builtin(e, "list", builtin_list);
    lenv_add_builtin(e, "head", builtin_head);
    lenv_add_builtin(e, "tail", builtin_tail);
    lenv_add_builtin(e, "join", builtin_join);

    /* Conditionals */
    lenv_add_builtin(e, "if", builtin_if);
    lenv_add_builtin(e, "==", builtin_eq);
    lenv_add_builtin(e, "!=", builtin_ne);
    lenv_add_builtin(e, ">", builtin_gt);
    lenv_add_builtin(e, "<", builtin_lt);
    lenv_add_builtin(e, ">=", builtin_ge);
    lenv_add_builtin(e, "<=", builtin_le);
    lenv_add_builtin(e, "or", builtin_or);
    lenv_add_builtin(e, "and", builtin_and);

    /* Strings */
    lenv_add_builtin(e, "load", builtin_load);
    lenv_add_builtin(e, "error", builtin_error);
    lenv_add_builtin(e, "print", builtin_print);

    /* Introspection */
    lenv_add_builtin(e, "type", builtin_type);
}

// Assert Macros {{{1
#define LASSERT(args, cond, fmt, ...) \
  if (!(cond)) { lval* err = lval_err(fmt, ##__VA_ARGS__); lval_del(args); return err; }

#define LASSERT_TYPE(func, args, index, expect) \
  LASSERT(args, args->cell[index]->type == expect, \
    "Function '%s' passed incorrect type for argument %i. " \
    "Got %s, Expected %s.", \
    func, index, lval_type_name(args->cell[index]->type), lval_type_name(expect))

#define LASSERT_NUM(func, args, num) \
  LASSERT(args, args->count == num, \
    "Function '%s' passed incorrect number of arguments. " \
    "Got %i, Expected %i.", \
    func, args->count, num)

#define LASSERT_NOT_EMPTY(func, args, index) \
  LASSERT(args, args->cell[index]->count != 0, \
    "Function '%s' passed {} for argument %i.", func, index);

// Utility {{{1

void lenv_add_builtin(lenv* e, char* name, lbuiltin func) {
    lval* k = lval_sym(name);
    lval* v = lval_builtin(func);
    lenv_put(e, k, v);
    lval_del(k);
    lval_del(v);
}

// Math {{{1
lval* builtin_math_op(lenv* e, lval* a, char* op) {
    for (int i = 0; i < a->count; i++) {
        LASSERT_TYPE(op, a, i, LVAL_NUM);
    }

    lval* x = lval_pop(a, 0);

    if ((strcmp(op, "-") == 0) && a->count == 0) {
        x->num = -x->num;
    }

    while (a->count > 0) {
        lval* y = lval_pop(a, 0);

        if (strcmp(op, "+") == 0) { x->num += y->num; }
        if (strcmp(op, "-") == 0) { x->num -= y->num; }
        if (strcmp(op, "*") == 0) { x->num *= y->num; }
        if (strcmp(op, "/") == 0) {
            if (y->num == 0) {
                lval_del(x);
                lval_del(y);
                x = lval_err("Division By Zero!"); break;
            }
            x->num /= y->num;
        }

        lval_del(y);
    }

    lval_del(a);
    return x;
}

lval* builtin_add(lenv* e, lval*a) {
    return builtin_math_op(e, a, "+");
}

lval* builtin_sub(lenv* e, lval*a) {
    return builtin_math_op(e, a, "-");
}

lval* builtin_mul(lenv* e, lval*a) {
    return builtin_math_op(e, a, "*");
}

lval* builtin_div(lenv* e, lval*a) {
    return builtin_math_op(e, a, "/");
}

// Lists {{{1
lval* builtin_head(lenv* e, lval* a) {
    LASSERT(a, a->count == 1, "fn head: expected %i, got %i parameters", 1, a->count);
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "expected qexpr, got %s", lval_type_name(a->cell[0]->type));
    LASSERT(a, a->cell[0]->count != 0, "got empty list", ' ');

    lval* v = lval_take(a, 0);

    while (v->count > 1) {
        lval_del(lval_pop(v, 1));
    }

    return v;
}

lval* builtin_tail(lenv* e, lval* a) {
    LASSERT(a, a->count == 1, "too many parameters", ' ');
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "expected qexpr", ' ');
    LASSERT(a, a->cell[0]->count != 0, "got empty list", ' ');

    lval* v = lval_take(a, 0);

    lval_del(lval_pop(v, 0));
    return v;
}

lval* builtin_list(lenv* e, lval* a) {
    a->type = LVAL_QEXPR;
    return a;
}

lval* builtin_join(lenv* e, lval* a) {
    for (int i = 0; i < a->count; i++) {
        LASSERT(a, a->cell[i]->type == LVAL_QEXPR, "incorrect type", ' ');
    }

    lval* x = lval_pop(a, 0);

    while (a->count > 0) {
        x = lval_join(x, lval_pop(a, 0));
    }

    lval_del(a);
    return x;
}


// Eval, Fns & Define {{{1
lval* builtin_def(lenv* e, lval* a) {
    return builtin_var(e, a, "def");
}

lval* builtin_put(lenv* e, lval* a) {
    return builtin_var(e, a, "=");
}

lval* builtin_eval(lenv* e, lval* a) {
    LASSERT(a, a->count == 1, "too many arguments", ' ');
    LASSERT(a, a->cell[0]->type == LVAL_QEXPR, "incorrect type", ' ');
    lval* x = lval_take(a, 0);
    x->type = LVAL_SEXPR;
    return lval_eval(e, x);
}

lval* builtin_lambda(lenv* e, lval* a) {
    LASSERT_NUM("lambda", a, 2);
    LASSERT_TYPE("lambda", a, 0, LVAL_QEXPR);
    LASSERT_TYPE("lambda", a, 1, LVAL_QEXPR);

    for (int i = 0; i < a->cell[0]->count; i++) {
        LASSERT(a, (a->cell[0]->cell[i]->type == LVAL_SYM),
                "Cannot define non-symbol. Got %s, expected %s.",
                lval_type_name(a->cell[0]->cell[i]->type), lval_type_name(LVAL_SYM));
    }

    lval* formals = lval_pop(a, 0);
    lval* body = lval_pop(a, 0);
    lval_del(a);

    return lval_lambda(formals, body);
}

lval* builtin_var(lenv* e, lval* a, char* func) {
    LASSERT_TYPE(func, a, 0, LVAL_QEXPR);

    lval* syms = a->cell[0];
    for (int i = 0; i < syms->count; i++) {
        LASSERT(a, (syms->cell[i]->type == LVAL_SYM),
                "Function %s cannot define non-symbol. "
                "Got %s, expected %s.", func,
                lval_type_name(syms->cell[i]->type), lval_type_name(LVAL_SYM));
    }

    LASSERT(a, (syms->count == a->count - 1),
            "function %s cannot define incorrect "
            "number of values to symbols. "
            "Expected %i, got %i", func, syms->count, a->count - 1);

    for (int i = 0; i < syms->count; i++) {
        if (strcmp(func, "def") == 0) {
            lenv_def(e, syms->cell[i], a->cell[i + 1]);
        }

        if (strcmp(func, "=") == 0) {
            lenv_put(e, syms->cell[i], a->cell[i + 1]);
        }
    }

    lval_del(a);
    return lval_sexpr();
}

// Conditionals {{{1

lval* builtin_gt(lenv* e, lval* a) {
    return builtin_ord(e, a, ">");
}

lval* builtin_lt(lenv* e, lval* a) {
    return builtin_ord(e, a, "<");
}

lval* builtin_ge(lenv* e, lval* a) {
    return builtin_ord(e, a, ">=");
}

lval* builtin_le(lenv* e, lval* a) {
    return builtin_ord(e, a, "<=");
}

lval* builtin_ord(lenv* e, lval* a, char* op) {
    LASSERT_NUM(op, a, 2);
    LASSERT_TYPE(op, a, 0, LVAL_NUM);
    LASSERT_TYPE(op, a, 1, LVAL_NUM);

    int r;
    if (strcmp(op, ">") == 0) {
        r = (a->cell[0]->num > a->cell[1]->num);
    } else if (strcmp(op, "<") == 0) {
        r = (a->cell[0]->num < a->cell[1]->num);
    } else if (strcmp(op, ">=") == 0) {
        r = (a->cell[0]->num >= a->cell[1]->num);
    } else if (strcmp(op, "<=") == 0) {
        r = (a->cell[0]->num <= a->cell[1]->num);
    } else { }

    lval_del(a);
    return lval_bool(r);
}

int lval_eq(lval* x, lval* y) {
    if (x->type != y->type) {
        return 0;
    }

    switch (x->type) {
        case LVAL_BOOL:
        case LVAL_NUM: return (x->num == y->num);

        case LVAL_ERR: return (strcmp(x->err, y->err) == 0);
        case LVAL_STR: return (strcmp(x->sym, y->str) == 0);
        case LVAL_SYM: return (strcmp(x->sym, y->sym) == 0);

        case LVAL_FUN:
                       if (x->builtin || y->builtin) {
                           return x->builtin == y->builtin;
                       } else {
                           return lval_eq(x->formals, y->formals)
                               && lval_eq(x->body, y->body);
                       }
        case LVAL_QEXPR:
        case LVAL_SEXPR:
                       if (x->count != y->count) {
                           return 0;
                       }

                       for (int i = 0; i < x->count; i++) {
                           if (!lval_eq(x->cell[i], y->cell[i])) {
                               return 0;
                           }
                       }

                       return 1;
    }

    return 0;
}

lval* builtin_cmp(lenv* e, lval* a, char* op) {
    LASSERT_NUM(op, a, 2);
    int r;
    if (strcmp(op, "eq") == 0) {
        r = lval_eq(a->cell[0], a->cell[1]);
    } else if (strcmp(op, "ne") == 0) {
        r = !lval_eq(a->cell[0], a->cell[1]);
    }

    lval_del(a);
    return lval_bool(r);
}

lval* builtin_eq(lenv* e, lval* a) {
    return builtin_cmp(e, a, "eq");
}

lval* builtin_ne(lenv* e, lval* a) {
    return builtin_cmp(e, a, "ne");
}

lval* builtin_if(lenv* e, lval* a) {
    LASSERT_NUM("if", a, 3);
    LASSERT_TYPE("if", a, 1, LVAL_QEXPR);
    LASSERT_TYPE("if", a, 2, LVAL_QEXPR);

    lval* x;
    a->cell[1]->type = LVAL_SEXPR;
    a->cell[2]->type = LVAL_SEXPR;

    if (lval_is_true(a->cell[0])) {
        x = lval_eval(e, lval_pop(a, 1));
    } else {
        x = lval_eval(e, lval_pop(a, 2));
    }

    lval_del(a);
    return x;
}

lval* builtin_and(lenv* e, lval* a) {
    for (int i = 0; i < a->count; i++) {
        if (!lval_is_true(a->cell[i])) {
            lval_del(a);
            return lval_bool(0);
        }
    }

    lval* x = lval_copy(a->cell[a->count-1]);
    lval_del(a);
    return x;
}

lval* builtin_or(lenv* e, lval* a) {
    for (int i = 0; i < a->count; i++) {
        if (lval_is_true(a->cell[i])) {
            lval* x = lval_copy(a->cell[i]);
            lval_del(a);
            return x;
        }
    }
    lval_del(a);
    return lval_bool(0);
}

int lval_is_true(lval* a) {
    switch (a->type) {
        case LVAL_BOOL:
        case LVAL_NUM: return !(a->num == 0);
        case LVAL_ERR: return 0; // Errors are always false. Does this make sense? Probably not.
        case LVAL_QEXPR: return a->count;
        case LVAL_SYM:
        case LVAL_FUN:
        case LVAL_SEXPR:
                       return 1;
        default:
                       return 0;
    }
}

// Load {{{1
lval* builtin_load(lenv* e, lval* a) {
    LASSERT_NUM("load", a, 1);
    LASSERT_TYPE("load", a, 0, LVAL_STR);

    FILE* f = fopen(a->cell[0]->str, "rb");
    if (f == NULL) {
        lval* err = lval_err("Could not load library %s", a->cell[0]->str);
        lval_del(a);
        return err;
    }

    fseek(f, 0, SEEK_END);
    long length = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* input = calloc(length+1, 1);
    fread(input, 1, length, f);
    fclose(f);

    int pos = 0;
    lval* expr = lval_read_expr(input, &pos, '\0');
    free(input);

    if (expr->type != LVAL_ERR) {
        while (expr->count) {
            lval* x = lval_eval(e, lval_pop(expr, 0));
            if (x->type == LVAL_ERR) {
                lval_println(x);
            }
            lval_del(x);
        }
    } else {
        lval_println(expr);
    }

    lval_del(expr);
    lval_del(a);

    return lval_sexpr();
}

// Print {{{1
lval* builtin_print(lenv* e, lval* a) {
    for (int i = 0; i < a->count; i++) {
        lval_print(a->cell[i]);
        putchar(' ');
    }

    putchar('\n');
    lval_del(a);

    return lval_sexpr();
}

lval* builtin_error(lenv* e, lval* a) {
    LASSERT_NUM("error", a, 1);
    LASSERT_TYPE("error", a, 0, LVAL_STR);

    lval* err = lval_err(a->cell[0]->str);

    lval_del(a);
    return err;
}

// Introspection {{{1
lval* builtin_type(lenv* e, lval* a) {
    LASSERT_NUM("type", a, 1);

    printf("%s", lval_type_name(a->cell[0]->type));
    return lval_sexpr();
}


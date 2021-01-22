#ifndef PARSER_H
#define PARSER_H

#include "lval.h"

lval* lval_read(char* s, int* i);
lval* lval_read_expr(char* s, int* i, char end);
lval* lval_read_sym(char* s, int* i);
lval* lval_read_str(char* s, int* i);
char lval_str_unescape(char x);
char* lval_str_escape(char x);
void lval_print_str(lval* v);

#endif /* PARSER_H */

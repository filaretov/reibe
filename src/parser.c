#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>

#include "parser.h"

static char* lval_str_unescapable = "abfnrtv\\\'\"";
static char* lval_str_escapable = "\a\b\f\n\r\t\v\\\'\"";

lval* lval_read(const char* s, int* i) {
    while (strchr(" \t\v\r\n;", s[*i]) && s[*i] != '\0') {
        if (s[*i] == ';') {
            while (s[*i] != '\n' && s[*i] != '\0') {
                (*i)++;
            }
        }
        (*i)++;
    }

    lval* x = NULL;

    if (s[*i] == '\0') {
        return lval_err("Unexpected EOI");
    } else if (s[*i] == '(') {
        (*i)++;
        x = lval_read_expr(s, i, ')');
    } else if (s[*i] == '[') {
        (*i)++;
        x = lval_read_expr(s, i, ']');
    } else if (strchr(
                "abcdefghijklmnopqrstuvwxyz"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "0123456789_+-*\\/=<>!&", s[*i])) {
        x = lval_read_sym(s, i);
    } else if (strchr("\"", s[*i])) {
        x = lval_read_str(s, i);
    } else {
        x = lval_err("Unexpected character %c", s[*i]);
    }

    while (strchr(" \t\v\r\n;", s[*i]) && s[*i] != '\0') {
        if (s[*i] == ';') {
            while (s[*i] != '\n' && s[*i] != '\0') {
                (*i)++;
            }
        }
        (*i)++;
    }

    return x;
}

lval* lval_read_expr(const char* s, int* i, char end) {
    lval* x = (end == ']') ? lval_qexpr() : lval_sexpr();
    while (s[*i] != end) {
        lval* y = lval_read(s, i);
        if (y->type == LVAL_ERR) {
            lval_del(x);
            return y;
        } else {
            lval_add(x, y);
        }
    }
    (*i)++;
    return x;
}

lval* lval_read_sym(const char* s, int* i) {
    char* part = calloc(1, 1);

    while (strchr(
                "abcdefghijklmnopqrstuvwxyz"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "0123456789_+-*\\/=<>!&", s[*i]) && s[*i] != '\0') {
        part = realloc(part, strlen(part)+2);
        part[strlen(part)+1] = '\0';
        part[strlen(part)+0] = s[*i];
        (*i)++;
    }

    int is_num = strchr("-0123456789", part[0]) != NULL;
    for (int j = 1; j < strlen(part); j++) {
        if (!strchr("0123456789", part[j])) {
            is_num = 0;
            break;
        }
    }

    if (strlen(part) == 1 && part[0] == '-') {
        is_num = 0;
    }

    lval* x = NULL;
    if (is_num) {
        errno = 0;
        long v = strtol(part, NULL, 10);
        x = (errno != ERANGE) ? lval_num(v) : lval_err("Invalid Number %s", part);
    } else {
        x = lval_sym(part);
    }

    free(part);
    return x;
}

lval* lval_read_str(const char* s, int* i) {
    char* part = calloc(1, 1);
    (*i)++;
    while(s[*i] != '"') {
        char c = s[*i];

        if (c == '\0') {
            free(part);
            return lval_err("Unexpected end of input at string literal");
        }

        if (c == '\\') {
            (*i)++;
            if (strchr(lval_str_unescapable, s[*i])) {
                c = lval_str_unescape(s[*i]);
            } else {
                free(part);
                return lval_err("Invalid escape character %c", c);
            }
        }

        part = realloc(part, strlen(part)+2);
        part[strlen(part)+1] = '\0';
        part[strlen(part)+0] = c;
        (*i)++;
    }
    // Move past final " char
    (*i)++;

    lval* x = lval_str(part);
    free(part);
    return x;
}

char lval_str_unescape(char x) {
    switch (x) {
        case 'a':  return '\a';
        case 'b':  return '\b';
        case 'f':  return '\f';
        case 'n':  return '\n';
        case 'r':  return '\r';
        case 't':  return '\t';
        case 'v':  return '\v';
        case '\\': return '\\';
        case '\'': return '\'';
        case '\"': return '\"';
    }
    return '\0';
}

char* lval_str_escape(char x) {
    switch (x) {
        case '\a': return "\\a";
        case '\b': return "\\b";
        case '\f': return "\\f";
        case '\n': return "\\n";
        case '\r': return "\\r";
        case '\t': return "\\t";
        case '\v': return "\\v";
        case '\\': return "\\\\";
        case '\'': return "\\\'";
        case '\"': return "\\\"";
    }
    return "";
}

void lval_print_str(lval* v) {
    putchar('"');
    for (int i = 0; i < strlen(v->str); i++) {
        if (strchr(lval_str_escapable, v->str[i])) {
            printf("%s", lval_str_escape(v->str[i]));
        } else {
            putchar(v->str[i]);
        }
    }
    putchar('"');
}

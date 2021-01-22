#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>
#include <editline/history.h>

#include "types.h"
#include "lenv.h"
#include "lval.h"
#include "builtins.h"
#include "parser.h"

#define loop while(1)
#define PROMPT "reibe> "

lenv* global_env = NULL;

void exit_reibe() {
    if (global_env) {
        lenv_del(global_env);
    }
    putchar('\n');
    exit(0);
}

void sigint_handler(int sig) {
    exit_reibe();
}

int main(int argc, char** argv) {
    puts("Stammer -- Version 2021.1.0");
    puts("Exit with Ctrl+C\n");

    signal(SIGINT, sigint_handler);
    global_env = lenv_new();
    lenv_add_builtins(global_env);
    if (argc >= 2) {
        for (int i = 1; i < argc; i++) {
            lval* args = lval_add(lval_sexpr(), lval_str(argv[i]));

            lval* x = builtin_load(global_env, args);

            if (x->type == LVAL_ERR) {
                lval_println(x);
            }

            lval_del(x);
        }
    }

    loop {
        char* input = readline(PROMPT);
        if (input == NULL) {
            exit_reibe();
            continue;
        }
        int pos = 0;
        add_history(input);
        lval* expr = lval_read_expr(input, &pos, '\0');

        lval* x = lval_eval(global_env, expr);
        lval_println(x);
        lval_del(x);

        free(input);
    }

    exit_reibe();
    return 0;
}


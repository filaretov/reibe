#include "greatest.h"

#include "parser.h"
#include "lval.h"
#include "lenv.h"
#include "builtins.h"

static const char* program_define_function = "(def {just1} (lambda {} {1}))";
static const char* program_run_function = "(just1)";
static const char* program_const_one = "1";


TEST parse_function_definition(void) {
    int i = 0;
    lenv* env = lenv_new();
    lenv_add_builtins(env);
    lval* parsed = lval_read_expr(program_define_function, &i, '\0');
    lval* x = lval_eval(env, parsed);
    ASSERT_EQ(x->type, LVAL_SEXPR);

    i = 0;
    parsed = lval_read_expr(program_run_function, &i, '\0');
    x = lval_eval(env, parsed);

    ASSERT_EQ(x->type, LVAL_NUM);
    PASS();
}

TEST parse_const_one(void) {
    int i = 0;
    lval* parsed = lval_read(program_const_one, &i);

    ASSERT_EQ(parsed->num, 1);
    PASS();
}

SUITE(example_programs_suite) {
    RUN_TEST(parse_function_definition);
    RUN_TEST(parse_const_one);
}

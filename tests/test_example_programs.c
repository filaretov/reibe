#include "greatest.h"

#include "parser.h"

static const char* program_define_function =
"(def {just1} (lambda {} 1))";


TEST parse_function_definition(void) {
    int i = 0;
    lval* parsed = lval_read(program_define_function, &i);
    ASSERT_STR_EQ(parsed->sym, "just1");

    PASS();
}

SUITE(example_programs_suite) {
    RUN_TEST(parse_function_definition);
}

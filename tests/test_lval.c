#include "greatest.h"
#include "lval.h"

TEST x_const_1(void) {
    lval* x = lval_num(1);
    ASSERT_EQ_FMT(1l, x->num, "lval num: %li");
    lval_del(x);
    PASS();
}

SUITE(lval_suite) {
    RUN_TEST(x_const_1);
}

#include "greatest.h"

extern SUITE(lval_suite);
extern SUITE(example_programs_suite);

GREATEST_MAIN_DEFS();

int main(int argc, char** argv)
{
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(lval_suite);

    GREATEST_MAIN_END();
}

SRC=src/builtins.c src/lenv.c src/lval.c src/parser.c
HEADERS=src/types.h src/lenv.h src/lval.h
MAIN=src/main.c

OUT=reibe
LIBS=-ledit -lm
FLAGS=-std=c11 -Wall -Werror -pedantic
CC=gcc
TEST_SRC=tests/test.c tests/test_lval.c tests/test_example_programs.c
TEST_BIN=./run_tests

.PHONY: clean

test:
	$(CC) $(FLAGS) $(TEST_SRC) $(SRC) -Isrc $(LIBS) -o $(TEST_BIN)
	$(TEST_BIN)

$(OUT): $(SRC) $(HEADERS)
	$(CC) $(FLAGS) $(MAIN) $(SRC) $(LIBS) -o $(OUT)

clean:
	rm $(OUT)

IN=src/main.c src/builtins.c src/lenv.c src/lval.c src/parser.c
HEADERS=src/types.h src/lenv.h src/lval.h
OUT=reibe
LIBS=-ledit -lm
FLAGS=-std=c11 -Wall
CC=gcc

.PHONY: clean

$(OUT): $(IN) $(HEADERS)
	$(CC) $(FLAGS) $(IN) $(LIBS) -o $(OUT)

clean:
	rm $(OUT)

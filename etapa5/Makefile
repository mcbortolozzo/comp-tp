


CC = gcc
CC_FLAGS = -g

EXEC = etapa5
SOURCES = $(filter-out y.tab.c lex.yy.c, $(wildcard *.c))
OBJECTS = $(SOURCES:.c=.o) lex.yy.o y.tab.o


EXEC: $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXEC)

lex.yy.c: scanner.l y.tab.c
	lex scanner.l

y.tab.c: parser.y
	yacc -d parser.y

%.o: %.c
	$(CC) -c $(CC_FLAGS) $< -o $@

clean:
	rm -f lex.yy.c y.tab.c y.tab.h $(EXEC) $(OBJECTS)

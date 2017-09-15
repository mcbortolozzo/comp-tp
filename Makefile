


CC = gcc
CC_FLAGS = -g

EXEC = etapa1
SOURCES = $(filter-out lex.yy.c, $(wildcard *.c))
OBJECTS = $(SOURCES:.c=.o) lex.yy.o


EXEC: $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXEC)

lex.yy.c: scanner.l
	lex scanner.l

%.o: %.c
	$(CC) -c $(CC_FLAGS) $< -o $@

clean:
	rm -f lex.yy.c $(EXEC) $(OBJECTS)
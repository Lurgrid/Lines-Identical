CC = gcc
CFLAGS = -std=c11 \
  -Wall -Wconversion -Werror -Wextra -Wpedantic -Wwrite-strings \
  -O2
LDFLAGS =
objects = ir.o ir_test.o
executable = ir_test

all: $(executable)

clean:
	$(RM) $(objects) $(executable)

$(executable): $(objects)
	$(CC) $(objects) $(LDFLAGS) -o $(executable)

dist: clean
	zip "$(CURDIR).zip" *.c *.h makefile

ir.o: ir.c ir.h
main.o: main.c ir.h

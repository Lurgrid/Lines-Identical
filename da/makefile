CC = gcc
CFLAGS = -std=c11 \
  -Wall -Wconversion -Werror -Wextra -Wpedantic -Wwrite-strings \
  -O2
LDFLAGS =
objects = da.o da_test.o
executable = da_test

all: $(executable)

clean:
	$(RM) $(objects) $(executable)

$(executable): $(objects)
	$(CC) $(objects) $(LDFLAGS) -o $(executable)

dist: clean
	zip "$(CURDIR).zip" *.c *.h makefile

da.o: da.c da.h
main.o: main.c da.h

CC = gcc
CFLAGS = -std=c11 \
  -Wall -Wconversion -Werror -Wextra -Wpedantic -Wwrite-strings \
  -O2
LDFLAGS =
objects = opt.o opt_test.o
executable = opt_test

all: $(executable)

clean:
	$(RM) $(objects) $(executable)

$(executable): $(objects)
	$(CC) $(objects) $(LDFLAGS) -o $(executable)

dist: clean
	zip "$(CURDIR).zip" *.c *.h makefile

opt.o: opt.c opt.h
opt_test.o: opt_test.c opt.h

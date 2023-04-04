bst_dir = ../bst/
op_dir = ../op/
scale_dir = ../scale/
CC = gcc
CFLAGS = -std=c18 \
  -Wall -Wconversion -Werror -Wextra -Wpedantic -Wwrite-strings \
  -O2 \
  -I$(bst_dir) -I$(op_dir) -I$(scale_dir)
vpath %.c $(bst_dir) $(op_dir) $(scale_dir)
vpath %.h $(bst_dir) $(op_dir) $(scale_dir)
objects = bst.o main.o op.o scale.o
executable = bstgame
makefile_indicator = .\#makefile\#

.PHONY: all clean

all: $(executable)

clean:
	$(RM) $(objects) $(executable)
	@$(RM) $(makefile_indicator)

$(executable): $(objects)
	$(CC) $(objects) -o $(executable)

bst.o: bst.c bst.h
main.o: main.c op.h scale.h bst.h
op.o: op.c op.h
scale.o: scale.c scale.h

include $(makefile_indicator)

$(makefile_indicator): makefile
	@touch $@
	@$(RM) $(objects) $(executable)

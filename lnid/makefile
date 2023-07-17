da_dir = ../da/
holdall_dir = ../holdall/
bst_dir = ../bst/
optl_dir = ../optl/
hashtable_dir = ../hashtable/
CC = gcc #Compiler name
CFLAGS = -std=c18 \
  -Wall -Wconversion -Werror -Wextra -Wpedantic -Wwrite-strings \
  -O2 -DHOLDALL_PUT_TAIL\
  -I$(holdall_dir) -I$(optl_dir) -I$(da_dir) -I$(hashtable_dir) -I$(bst_dir)
vpath %.c $(da_dir) $(holdall_dir) $(optl_dir) $(hashtable_dir) $(bst_dir)
vpath %.h $(da_dir) $(holdall_dir) $(optl_dir) $(hashtable_dir) $(bst_dir)
objects = optl.o da.o holdall.o hashtable.o bst.o lnid.o
executable = lnid
makefile_indicator = .\#makefile\#

.PHONY: all clean

all: $(executable)

clean:
	$(RM) $(objects) $(executable)
	@$(RM) $(makefile_indicator)

$(executable): $(objects)
	$(CC) $(objects) -o $(executable)

optl.o: optl.c optl.h
da.o: da.c da.h
holdall.o: holdall.c holdall.h
hashtable.o: hashtable.c hashtable.h
bst.o: bst.c bst.h
lnid.o: lnid.c optl.h holdall.h da.h

include $(makefile_indicator)

$(makefile_indicator): makefile
	@touch $@
	@$(RM) $(objects) $(executable)

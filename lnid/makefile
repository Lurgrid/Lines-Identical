da_dir = ../da/
holdall_dir = ../holdall/
opt_dir = ../opt/
CC = gcc
CFLAGS = -std=c18 \
  -Wall -Wconversion -Werror -Wextra -Wpedantic -Wwrite-strings \
  -O2 \
  -I$(holdall_dir) -I$(opt_dir) -I$(da_dir) 
vpath %.c $(da_dir) $(holdall_dir) $(opt_dir)
vpath %.h $(da_dir) $(holdall_dir) $(opt_dir)
objects = opt.o da.o holdall.o lnid.o
executable = lnid
makefile_indicator = .\#makefile\#

.PHONY: all clean

all: $(executable)

clean:
	$(RM) $(objects) $(executable)
	@$(RM) $(makefile_indicator)

$(executable): $(objects)
	$(CC) $(objects) -o $(executable)

opt.o: opt.c opt.h
da.o: da.c da.h
holdall.o: holdall.c holdall.h
lnid.o: lnid.c opt.h holdall.h da.h

include $(makefile_indicator)

$(makefile_indicator): makefile
	@touch $@
	@$(RM) $(objects) $(executable)

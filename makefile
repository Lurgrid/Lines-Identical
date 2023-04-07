.PHONY: clean dist

name ?= unknown

dist: clean
	tar -hzcf "$(name).tar.gz" bst/* da/* hashtable/* holdall/* lnid/* opt/* makefile

clean:
	$(MAKE) -C lnid clean

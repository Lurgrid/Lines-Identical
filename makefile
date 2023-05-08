.PHONY: clean dist

name ?= unknown

dist: clean
	tar -hzcf "$(name).tar.gz" bst/* da/* hashtable/* holdall/* lnid/* optl/* makefile

clean:
	$(MAKE) -C lnid clean
	$(MAKE) -C optl clean

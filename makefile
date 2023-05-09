.PHONY: clean dist

name ?= unknown

dist: clean
	tar -hzcf "$(name).tar.gz" bst/* da/* hashtable/* holdall/* lnid/* optl/* makefile compte-rendu/compte-rendu.pdf

clean:
	$(MAKE) -C lnid clean
	$(MAKE) -C optl clean

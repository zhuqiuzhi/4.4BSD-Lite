#Build for 4.4BSD

all:
# Currently support 'hp300', 'sparc'
	rm -f config.h
	ln -s config.${MACHINE}bsd.h config.h
	make -f Makefile

clean:
	make -f Makefile clean

install:
	make -f Makefile install
 in making a distribution.
#Do not use it on development directories!
distclean:
	-rm -f paths.h config.h emacs-* temacs xemacs xmakefile core \
   *~ \#* *.o libXMenu11.a log*

clean:
	-rm -f temacs xemacs xmakefile core \#* *.o libXMenu11.a

xemacs: xmakefile doxemacs

doxemacs:
	$(MAKE) $(MAKEOVERRIDES) -f xmakefile ${MFLAGS} xemacs

temacs: xmakefile dotemacs

dotemacs:
	$(MAKE) $(MAKEOVERRIDES) -f xmakefile ${MFLAGS} temacs

# If you have a problem with cc -E here, changing
# the definition of CPP above may fix it.
xmakefile: ymakefile config.h
	-rm -f xmakefile junk.c
	cp ymakefile junk.c
	$(CPP) -DYMAKEFILE junk.c | \
	sed -e 's/^#.*//' -e 's/^[ 	][ 	]*$$//' -e 's/^ /	/' | \
	sed -n -e '/^..*$$/p' > xmakefile
	rm -f junk.c

tags:
	etags [a-z]*.h [a-z]*.c ../lisp/[a-z]*.el

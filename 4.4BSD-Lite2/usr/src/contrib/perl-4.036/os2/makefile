#
# Makefile for compiling Perl under OS/2
#
# Needs Microsoft C 6.00 and NMAKE
#

EXP =   c:\ms\lib\setargv.obj -link /noe

DEF =   os2\perl.def
BAD =   os2\perl.bad

OBJ =	array.obj cmd.obj cons.obj consarg.obj doarg.obj doio.obj \
	dolist.obj dump.obj eval.obj form.obj hash.obj perl.obj perly.obj \
	regcomp.obj regexec.obj stab.obj str.obj toke.obj util.obj
OBJO =	os2.obj popen.obj suffix.obj director.obj alarm.obj crypt.obj

LIBS =  lgdbm.lib

YACC=bison
YFLAGS=-d

CC=cl -nologo
CCL=cl -nologo -B2C2L -B3C3L

CFLAGS=-W1 -AL -Zep -J -G2s -Olt -Gt 2048 -DDEBUGGING
#CFLAGS=-W1 -AL -Ziep -J -G2 -Od -Gt 2048 -DDEBUGGING

LDFLAGS=-AL -Lp -F 8000
#LDFLAGS=-AL -Lp -Zi -Li -F 8000

STRIP=bind -nologo
#STRIP=rem

.c.obj:
        $(CC) -c $(CFLAGS) $<

{os2}.c{}.obj:
        $(CC) -c $(CFLAGS) -I. -Ios2 $<

all: perl.exe perlglob.exe

perl.exe: $(OBJ) $(OBJO)
        $(CC) $(LDFLAGS) $(OBJ) $(OBJO) $(LIBS) os2\perl.def -o $@ $(EXP)
        $(STRIP) $@ -n @$(BAD)

$(OBJ) $(OBJO): config.h
perl.obj str.obj cons.obj toke.obj: perly.h

config.h: os2\config.h
        cp os2\config.h config.h

perly.c perly.h: perly.y
        $(YACC) $(YFLAGS) -o $*.c $*.y

eval.obj:	eval.c
	$(CCL) -c $(CFLAGS) $*.c
toke.obj:	toke.c
	$(CCL) -c $(CFLAGS) $*.c

perlglob.exe: os2\glob.c os2\director.c
	$(CC) -Zep -G2s -Olt -Lp os2\glob.c $(DEF) -o $@ $(EXP)
        $(STRIP) $@ -n @$(BAD)

clean:
        -rm perly.c perly.h config.h *.obj >nul
ld:  Optimization on, symbols stripped.
CBASE=-AL -Zi -G2 -Gs -DDEBUGGING
CPLAIN=$(CBASE) -Olt
CFLAGS=$(CBASE) -Oeglt
PERL=perl.exe

# Destination directory for executables
DESTDIR=\usr\bin

# Deliverables
#
all: $(PERL) glob.exe

perl.exe: $(OBJ) perl.arp
	link @perl.arp,perl,nul,/stack:32767 /NOE;
	exehdr /nologo /newfiles /pmtype:windowcompat perl.exe >nul

perlsym.exe: $(OBJ) perl.arp
	link @perl.arp,perlsym,nul,/stack:32767 /NOE /CODE;
	exehdr /nologo /newfiles /pmtype:windowcompat perlsym.exe >nul

perl.arp:
	echo array+cmd+cons+consarg+doarg+doio+dolist+dump+ >perl.arp
	echo eval+form+hash+perl+perly+regcomp+regexec+stab+suffix+ >>perl.arp
	echo str+toke+util+os2+popen+director+\c600\lib\setargv >>perl.arp

glob.exe: glob.c
	$(CC) glob.c setargv.obj -link /NOE
	exehdr /nologo /newfiles /pmtype:windowcompat glob.exe >nul

array.obj: array.c
	$(CC) $(CPLAIN) -c array.c
cmd.obj: cmd.c
cons.obj: cons.c perly.h
consarg.obj: consarg.c
#	$(CC) $(CPLAIN) -c consarg.c
doarg.obj: doarg.c
doio.obj: doio.c
dolist.obj: dolist.c
dump.obj: dump.c
eval.obj: eval.c evalargs.xc
	$(CC) /B2c2l /B3c3l $(CFLAGS) -c eval.c
form.obj: form.c
hash.obj: hash.c
perl.obj: perl.y
perly.obj: perly.c
regcomp.obj: regcomp.c
regexec.obj: regexec.c
stab.obj: stab.c
	$(CC) $(CPLAIN) -c stab.c
str.obj: str.c
suffix.obj: suffix.c
toke.obj: toke.c
	$(CC) /B3c3l $(CFLAGS) -c toke.c
util.obj: util.c
#	$(CC) $(CPLAIN) -c util.c
perly.h: ytab.h
	cp ytab.h perly.h
director.obj: director.c
popen.obj: popen.c
os2.obj: os2.c

perl.1: $(MAN)
	nroff -man $(MAN) >perl.1

install: all
	exepack perl.exe $(DESTDIR)\perl.exe
	exepack glob.exe $(DESTDIR)\glob.exe

clean:
	rm -f *.obj *.exe perl.1 perly.h perl.arp

tags:
	ctags *.c *.h *.xc

dosperl:
	mv $(DOSFILES) ../perl30.new

doskit:
	mv $(DOSFILES) ../os2

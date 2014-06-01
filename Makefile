# these defines are for crosscompiling a windows binary of hnb 

PDCURSES_PATH=/usr/local/cross-tools/i386-mingw32/lib
MINGW_PATH=/usr/local/cross-tools/i386-mingw32/bin

C_FLAGS=-Wall -pedantic -ansi -O2

BINFILES=hnb/hnb hnb/LICENSE hnb/README hnb/README.html hnb/hnb.1
WINFILES=hnb/hnb.exe hnb/LICENSE hnb/README hnb/README.html hnb/hnb.1

hnb: hnb.o file.o node.o tree.o ui.o path.o tokenizer.o
	gcc -o hnb hnb.o file.o node.o tree.o ui.o path.o tokenizer.o -lcurses
	strip hnb
hnb.o: hnb.c node.h tree.h file.h ui.h version.h
	gcc $(C_FLAGS) -c hnb.c
node.o: node.h node.c
	gcc $(C_FLAGS) -c node.c 
tree.o: node.h tree.c tree.h
	gcc $(C_FLAGS) -c tree.c 
file.o: file.c node.h tree.h file.h tutorial.inc
	gcc $(C_FLAGS) -c file.c 
ui.o: ui.c node.c tree.c ui.h
	gcc $(C_FLAGS) -c ui.c 
path.o: path.c node.h tree.h tokenizer.h
	gcc $(C_FLAGS) -c path.c 
tokenizer.o: tokenizer.c tokenizer.h
	gcc $(C_FLAGS) -c tokenizer.c
registry.o: registry.c node.h tree.h file.h path.h registry.h
	gcc $(C_FLAGS) -c registry.c
reg.o: reg.c node.h tree.h file.h path.h registry.h
	gcc $(C_FLAGS) -c reg.c
reg: tokenizer.o file.o path.o tree.o node.o registry.o reg.o
	gcc -o reg file.o node.o tree.o tokenizer.o path.o registry.o reg.o
version.h: VERSION
	echo \#define VERSION \" hierarchical notebook ver. `cat VERSION`\">version.h
clean: 
	rm -rf *.o *~ hnb hnb.exe reg DEADJOE cgi-bin core a.out
strip: hnb
	strip hnb
all: strip hnb.exe reg
run: hnb
	./hnb
dist-src: clean
	( cd .. ; tar -cvoyf dist/hnb-`cat hnb/VERSION`-src.tar.bz2 hnb ;)
dist-win: hnb.exe
	( cd .. ; zip dist/hnb-`cat hnb/VERSION`-win32.zip $(WINFILES);)
dist-bin: hnb
	( cd .. ; tar -cvoyf dist/hnb-`cat hnb/VERSION`-i386.tar.bz2 $(BINFILES);)
dist: dist-win dist-bin dist-src
hnb.exe: ui.c hnb.c node.c tree.c file.c version.h tree.h node.h ui.h file.h tokenizer.c tokenizer.h path.c path.h
	(export PATH=$(MINGW_PATH):$(PATH);\
	gcc $(C_FLAGS) -DWIN32 -o hnb.exe ui.c hnb.c node.c tree.c file.c path.c tokenizer.c $(PDCURSES_PATH)/pdcurses.a;\
	strip hnb.exe;)
install: hnb
	cp hnb /usr/bin
	cp hnb.1 /usr/man/man1/hnb.1


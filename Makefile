C_FLAGS=-Wall -pedantic -ansi

hnb: app.o file.o node.o tree.o ui.o
	gcc -o hnb *.o -lncurses
app.o: app.c node.h tree.h file.h ui.h version.h
	gcc $(C_FLAGS) -c app.c
node.o: node.h node.c
	gcc $(C_FLAGS) -c node.c
tree.o: node.h tree.c
	gcc $(C_FLAGS) -c tree.c
file.o: file.c node.h tree.h
	gcc $(C_FLAGS) -c file.c
ui.o: ui.c node.c tree.c ui.h
	gcc $(C_FLAGS) -c ui.c
path.o: path.c node.h tree.h
	gcc $(C_FLAGS) -c path.c 
clean: 
	rm -rf *.o *~ hnb
strip: hnb
	strip hnb
	rm -rf *.o *~
run: hnb
	./hnb
dist: clean
	( cd .. ; tar -cvozf hnb.tar.gz hnb )
indent-pip: 
	indent -npcs -lp -bls -bl -br -ce -cli4 -cbi4 -ci4 *.c
indent:
	indent *.c

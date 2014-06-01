CFLAGS = -Wall -O3

OBJS = cli.o cli_history.o 

all: libcli.a libcli.so test-static test-shared 

clean:
	rm -f *.o *.a *.so test-s* *~
libcli.a: $(OBJS)
	ar rcs libcli.a cli*.o
libcli_p.a: 
	cc -pg -c cli.c
	cc -pg -c cli_history.c
	ar rcs libcli_p.a cli*.o
libcli.so: $(OBJS)
	cc -shared cli*.o -o libcli.so $(CFLAGS) $(LIBS)
test-static: test.o libcli.a
	cc -o test-static test.o libcli.a $(CFLAGS) $(LIBS) 
test-shared: test.o libcli.so
	cc -o test-shared test.o -L. -lcli $(CFLAGS) $(LIBS) 

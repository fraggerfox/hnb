all: src/hnb

src/hnb: src/*.c src/*.h src/*.inc src/hnbrc.inc
	(cd src;${MAKE})
install: src/hnb
	install -D src/hnb /usr/local/bin/hnb
	install -D -m444 doc/hnb.1 /usr/local/man/man1/hnb.1
clean:
	(cd src;${MAKE} clean)
	(cd util;${MAKE} clean)
	rm -f *~

rcupdate: updaterc
updaterc: src/hnbrc.inc

src/hnbrc.inc: doc/hnbrc
	(cd util;${MAKE})
	echo -n "\"">src/hnbrc.inc
	cat doc/hnbrc | util/asc2c >> src/hnbrc.inc
	echo "\"">>src/hnbrc.inc


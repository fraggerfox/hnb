
src/hnb: src/*.c src/*.h src/*.inc
	(cd src;make)
install: src/hnb
	install src/hnb /usr/local/bin/
	install doc/hnb.1 /usr/local/man/man1/
clean:
	(cd src;make clean)
	(cd util;make clean)
	rm -f *~
rcupdate: updaterc
updaterc:
	(cd util;make)
	echo -n "\"">src/hnbrc.inc
	cat doc/hnbrc | util/asc2c >> src/hnbrc.inc
	echo "\"">>src/hnbrc.inc


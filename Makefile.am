EXTRA_DIST = configure reconf Makefile.win32
SUBDIRS = src doc util

hnb.exe: Makefile.win32
	make -f Makefile.win32 hnb.exe

windist: win-dist

distwin: dist-win

win-dist: dist-win

dist-win:
	make -f Makefile.win32 dist

# to make it much easier for me to take snapshots,.. 
scp: distwin dist
	scp1 hnb-*.tar.gz hnb-*-win32.zip pippin@hnb.sourceforge.net:www/files

# yet more convenience for me,.. for updating the website	
webupdate: www/*
	scp1 www/* pippin@hnb.sourceforge.net:www/
	touch webupdate

util/asc2c:
	(cd util;make asc2c;)

rc: doc/hnbrc util/asc2c
	echo -n \">src/hnbrc.inc
	cat doc/hnbrc | util/asc2c >>src/hnbrc.inc
	echo \">>src/hnbrc.inc

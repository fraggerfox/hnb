EXTRA_DIST = README.html ../ChangeLog hnb.1 ../AUTHORS hnbrc


README.html : ../src/hnb ../README
	../src/hnb -a ../README "export -h README.html"

../ChangeLog : ../src/hnb ../README
	../src/hnb -a ../README "cd CHANGELOG" "export -a ../ChangeLog"

../AUTHORS : ../src/hnb ../README
	../src/hnb -a ../README "cd AUTHORS" "export -a ../AUTHORS"

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(mandir)/man1
	$(INSTALL_DATA) $(srcdir)/hnb.1 $(DESTDIR)$(mandir)/man1/

uninstall-local:
	-rm -f $(DESTDIR)$(mandir)/man1/hnb.1

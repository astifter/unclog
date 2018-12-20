ACLOCAL_AMFLAGS = -Im4

SUBDIRS = src
EXTRA_DIST = autogen.sh unclog.doxygen README.md

# valgrind testing
valgrind gdb:
	$(MAKE) -C src/tests $@

# documentation and formating
html-local: format documentation/html/index.html

install-html-local: html
	install -d $(DESTDIR)$(docdir)
	cp -r ./documentation/html/* $(DESTDIR)$(docdir)

uninstall-local:
	rm -rf $(DESTDIR)$(docdir)
	rm -rf $(DESTDIR)$(includedir)/unclog

clean-local:
	rm -rf ./documentation/

format: 
	SOURCES=$$(find src/unclog src/tests -name '*.c'); \
	HEADERS=$$(find src/unclog src/tests -name '*.h'); \
	clang-format-3.8 -i -style=file $${SOURCES} $${HEADERS}

documentation/html/index.html: unclog.doxygen src/unclog/*.c src/unclog/*.h README.md
	doxygen $< 

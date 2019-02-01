ACLOCAL_AMFLAGS = -Im4

SUBDIRS = src
EXTRA_DIST = autogen.sh unclog.doxygen README.md

# valgrind testing
valgrind gdb: ./src/unclog/.libs/libunclog.so
	$(MAKE) -C src/tests $@

examples: check
	$(MAKE) -C src/examples $@

# documentation and formating
html-local: format docs/html/index.html

install-html-local: html
	install -d $(DESTDIR)$(docdir)
	cp -r ./docs/html/* $(DESTDIR)$(docdir)

uninstall-local:
	rm -rf $(DESTDIR)$(docdir)
	rm -rf $(DESTDIR)$(includedir)/unclog

clean-local:
	rm -rf ./docs/

format: 
	SOURCES=$$(find src/unclog src/tests -name '*.c'); \
	HEADERS=$$(find src/unclog src/tests -name '*.h'); \
	clang-format-3.8 -i -style=file $${SOURCES} $${HEADERS}

docs/html/index.html: unclog.doxygen src/unclog/*.c src/unclog/*.h README.md
	doxygen $< 

abi-check: abi-dumps/unclog-latest.dump
	abi-compliance-checker -l unclog -old abi-dumps/unclog-$(PREVIOUS_VERSION).dump -new $<

abi-dumps/unclog-latest.dump: ./src/unclog/.libs/libunclog.so
	abi-dumper -o $@ -vnum $(VERSION) -public-headers ./src/unclog/unclog $<

./src/unclog/.libs/libunclog.so: all

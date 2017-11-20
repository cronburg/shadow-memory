
all:
	$(MAKE) -C src/ all

html/index.html: Doxyfile src/shadow.o
	doxygen Doxyfile


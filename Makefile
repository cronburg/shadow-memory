
INCLUDE=.
LIB=.
EXE=main

all: $(EXE)

shadow.o: shadow.c shadow.h
	gcc -g -I${INCLUDE} -c -Wall -Werror -fpic shadow.c

shadow.so: shadow.o
	gcc -g -shared -o libshadow.so shadow.o

main: main.c shadow.so
	gcc -g -I${INCLUDE} -L${LIB} -Wall -Werror -o main main.c -lshadow

clean:
	rm -f *.o *.so $(EXE)


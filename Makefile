
INCLUDE=.
LIB=.
EXE=main merge

all: $(EXE)

shadow.o: shadow.c shadow.h shadow-lib.c shadow-lib.h shadow_private.h
	gcc -g -I${INCLUDE} -c -Wall -Werror -fpic shadow.c

shadow.so: shadow.o
	gcc -g -shared -o libshadow.so shadow.o

main: main.c shadow.h shadow.so
	gcc -g -I${INCLUDE} -L${LIB} -Wall -Werror -o main main.c -lshadow

merge: merge.c shadow.h shadow.so
	gcc -g ${FLAGS} -I${INCLUDE} -L${LIB} -Wall -o merge merge.c -lshadow

clean:
	rm -f *.o *.so $(EXE)


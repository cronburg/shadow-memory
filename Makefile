
INCLUDE=.
LIB=.
EXE=main merge

TARGET=ia32
#TARGET=intel64

ifeq ($(TARGET),ia32)
ARCH_FLAGS := -m32
else
ARCH_FLAGS := 
endif

all: $(EXE)

shadow.o: shadow.c shadow.h shadow-lib.c shadow-lib.h shadow_private.h
	gcc $(ARCH_FLAGS) -g -I${INCLUDE} -c -Wall -Werror -fpic shadow.c

shadow.so: shadow.o
	gcc $(ARCH_FLAGS) -g -shared -o libshadow.so shadow.o

main: main.c shadow.h shadow.so
	gcc $(ARCH_FLAGS) -g -I${INCLUDE} -L${LIB} -Wall -Werror -o main main.c -lshadow

merge: merge.c shadow.h shadow.so
	gcc $(ARCH_FLAGS) -g ${FLAGS} -I${INCLUDE} -L${LIB} -Wall -o merge merge.c -lshadow

clean:
	rm -f *.o *.so $(EXE)


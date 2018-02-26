
INCLUDE=.
LIB=.
EXE=main merge main-opt merge-opt main-64 main-32

# Default to intel64 target:
ifndef $(TARGET)
#TARGET=ia32
TARGET=intel64
endif

ifeq ($(TARGET),ia32)
ARCH_FLAGS := -m32
else
ARCH_FLAGS := 
endif

all: $(EXE)

shadow.o: shadow.c shadow.h shadow-lib.h shadow_private.h
	gcc $(ARCH_FLAGS) -g -I${INCLUDE} -c -Wall -Werror -fpic shadow.c

shadow.so: shadow.o
	gcc $(ARCH_FLAGS) -g -shared -o libshadow.so shadow.o

shadow-64.o: shadow-64.c shadow-64.h
	gcc $(ARCH_FLAGS) -g -I${INCLUDE} -c -Wall -Werror -fpic shadow-64.c

shadow-32.o: shadow-32.c shadow-32.h
	gcc -m32 $(ARCH_FLAGS) -g -I${INCLUDE} -c -Wall -Werror -fpic shadow-32.c

##############################################################################
# Compile without inlining (via shared object file):
main: main.c shadow.h shadow.so
	gcc $(ARCH_FLAGS) -g -I${INCLUDE} -L${LIB} -Wall -Werror -o main main.c -lshadow

merge: merge.c shadow.h shadow.so
	gcc $(ARCH_FLAGS) -g ${FLAGS} -I${INCLUDE} -L${LIB} -Wall -o merge merge.c -lshadow

##############################################################################
# Compile with inlining:
main-opt: main.c shadow.h
	gcc $(ARCH_FLAGS) -g -I${INCLUDE} -Wall -Werror -o main-opt main.c

merge-opt: merge.c shadow.h
	gcc $(ARCH_FLAGS) -g ${FLAGS} -I${INCLUDE} -Wall -o merge-opt merge.c

##############################################################################
# Compile 64 and 32 bit mmap versions:
main-64: main-64.c shadow-64.o
	gcc $(ARCH_FLAGS) -g ${FLAGS} -I${INCLUDE} -Wall shadow-64.o -o main-64 main-64.c

main-32: main-32.c shadow-32.o
	gcc -m32 ${FLAGS} -I${INCLUDE} -Wall shadow-32.o -o main-32 -Og -ggdb3 main-32.c

clean:
	rm -f *.gch *.o *.so $(EXE)


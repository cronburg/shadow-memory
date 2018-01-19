
INCLUDE=.
LIB=.
EXE=main merge main-opt merge-opt main-mmap

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
# Compile 64-bit mmap version:
main-mmap: main-mmap.c shadow-mmap.h
	gcc $(ARCH_FLAGS) -g ${FLAGS} -I${INCLUDE} -Wall -o main-mmap main-mmap.c

clean:
	rm -f *.gch *.o *.so $(EXE)


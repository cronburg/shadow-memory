/**
 * API / operations to perform on a shadow memory map  (based on memcheck)
 **/

#ifndef shadow_h__
#define shadow_h__

#include <stddef.h>

// The possible states of memory:
#define DATA_META    0x0 // 00 binary
#define DATA_OBJECT  0x1 // 01 binary
#define DATA_UNDEF   0x2 // 10 binary

#define KB_64        65536

#define INLINE    inline __attribute__((always_inline))

typedef unsigned char U8; //UChar;
typedef int Addr;

// Secondary Map structure
typedef struct {
  U8 mbits[KB_64];
} SM;

// Whether or not the given SM is the "distinguished" secondary map
// UNDEFINED DSM is what all PM entries point to initially before any
// memory accesses occur. 
int is_DSM(SM* sm);

// Allocates and initializes a new SM
SM* copy_for_writing(SM* sm);

// Four primary shadow map operations
SM* get_SM_for_reading(Addr a);
SM* get_SM_for_writing(Addr a);
void get_meta_bits(Addr a, U8* mbits);
void set_meta_bits(Addr a, U8  mbits);

// Application needs to explicitly determine how system calls are made
extern void* shadow_malloc(size_t size);
extern void  out_of_memory();
extern void  mem_copy(void* dst, void* src, size_t size);

#endif // shadow_h__

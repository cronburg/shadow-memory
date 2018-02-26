#ifndef shadow_mmap_h__
#define shadow_mmap_h__
#include <stddef.h> 	// size_t
#include <assert.h> 	// assert()
#include <string.h> 	// memcpy()
#include <sys/mman.h> // MAP_FAILED
#include <stdio.h>    // perror()

typedef int Int;
typedef size_t SizeT;
typedef unsigned long long int Addr;
typedef unsigned char U8;

// Application needs to explicitly determine how system calls are made
void  shadow_free(void* addr);
void* shadow_malloc(SizeT size);
void* shadow_calloc(SizeT nmemb, SizeT size);
void  shadow_out_of_memory();
void  shadow_memcpy(void* dst, void* src, SizeT size);

// >>> hex(2**35 - 1)
//#define LOW_HALF_BITS 0x00000007ffffffff

// >>> hex(2**18 - 1)
#define LOW_BITS 0x3ffff
typedef struct { U8 bits[262144]; } Low; // 2**18 bytes (bit for bit shadow map)

// >>> hex(2**(17+18) - 1 - (2**18 - 1))
#define MIDDLE_BITS 0x7fffc0000
typedef struct { Low* bits[131072]; } Middle; // 2**17 pointers

typedef union {
  Middle* m;
  Low* l;
} MiddleLowPtr;

// >>> hex(2**64 - 2**(17+18))
#define HIGH_BITS 0xfffffff800000000
typedef struct { MiddleLowPtr bits[536870912]; } High; // 2**29 pointers

// >>> hex(0xfffffff800000000 + 0x7fffc0000 + 0x3ffff)
// '0xffffffffffffffff'

#define HIGH_SIZE 4294967296
#define LOW_SIZE (sizeof(Low))
#define MID_SIZE (sizeof(Middle))
#define SHADOW_INIT_SIZE (HIGH_SIZE + LOW_SIZE + MID_SIZE)

typedef struct {
  High* map;              // pointer to the primary shadow map
  Low* distinguished_map; // pointer to the (only) distinguished map
  Middle* distinguished_middle;
} ShadowMap;

// ----------------------------------------------------------------------------
#ifndef DEBUG
#define INLINE    inline __attribute__((always_inline))
#else
#define INLINE
#endif

// Macros for accessing map fields of primary shadow map struct:
#define BITS(PM)   (PM->map->bits)
#define DMAP(PM)  (PM->distinguished_map)

// Allocates and initializes a new Low
Low* copy_for_writing(Low* low);

// Secondary Map getters (maps application address to which Low that address is in)
Low* get_Low_for_reading(ShadowMap *PM, Addr a);
Low* get_Low_for_writing(ShadowMap *PM, Addr a);

void shadow_get_bits(ShadowMap *PM, Addr a, U8* mbits);
void shadow_set_bits(ShadowMap *PM, Addr a, U8  mbits);
void shadow_mark_bit(ShadowMap *PM, Addr a, U8 offset);
void shadow_unmark_bit(ShadowMap *PM, Addr a, U8 offset);
void shadow_get_bit(ShadowMap *PM, Addr a, U8 offset, U8* bit);
void shadow_initialize_map(ShadowMap* PM);
void shadow_initialize_with_memory(Addr mem, ShadowMap* PM);
void shadow_initialize_with_mmap(ShadowMap* PM);
void shadow_destroy_map(ShadowMap* PM);

#endif // shadow_mmap_h__

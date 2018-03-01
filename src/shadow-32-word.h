#ifndef shadow_32_word_h__
#define shadow_32_word_h__
#include <stddef.h>   // size_t
#include <assert.h>   // assert()
#include <string.h>   // memcpy()
#include <sys/mman.h> // MAP_FAILED
#include <stdio.h>    // perror()

typedef int Int;
typedef size_t SizeT;
typedef unsigned long int Addr;
typedef unsigned char U8;
typedef unsigned long int U32;

// Application needs to explicitly determine how system calls are made
void  shadow_free(void* addr);
void* shadow_malloc(SizeT size);
void* shadow_calloc(SizeT nmemb, SizeT size);
void  shadow_out_of_memory();
void  shadow_memcpy(void* dst, void* src, SizeT size);

// >>> hex(2**14 - 1)
#define NUM_LOW_BITS 14
#define LOW_BITS 0x3fff
#define LOW_COUNT 16384 // 4 * 2**14 bytes
typedef struct { U32 bits[LOW_COUNT]; } Low; // 4 * 2**14 bytes (4 byte shadow map for every app byte)

// >>> hex(2**32 - 2**14)
#define NUM_HIGH_BITS (32 - NUM_LOW_BITS) // 18
#define HIGH_BITS 0xffffc000
#define HIGH_COUNT 262144
typedef struct { Low* bits[HIGH_COUNT]; } High; // 2**18 pointers

#define HIGH_SIZE (HIGH_COUNT * sizeof(Low*)) // 1048576
#define LOW_SIZE (sizeof(Low))
#define SHADOW_INIT_SIZE (HIGH_SIZE + LOW_SIZE)

typedef struct {
  High* map;              // pointer to the primary shadow map
  Low* distinguished_map; // pointer to the (only) distinguished map
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
Low* word_copy_for_writing(Low* low);

// Secondary Map getters (maps application address to which Low that address is in)
Low* word_get_Low_for_reading(ShadowMap *PM, Addr a);
Low* word_get_Low_for_writing(ShadowMap *PM, Addr a);

Low* word_copy_for_writing(Low* dist_Low);

Low* word_get_Low_for_reading(ShadowMap *PM, Addr a);
Low* word_get_Low_for_writing(ShadowMap *PM, Addr a);
void shadow_word_get_bits(ShadowMap *PM, Addr a, U32* mbits);
void shadow_word_set_bits(ShadowMap *PM, Addr a, U32  mbits);
void shadow_word_mark_bit(ShadowMap *PM, Addr a, U8  offset);
void shadow_word_unmark_bit(ShadowMap *PM, Addr a, U8 offset);
void shadow_word_get_bit(ShadowMap *PM, Addr a, U8 offset, U32* bit);
void shadow_word_initialize_map(ShadowMap* PM);
void shadow_word_initialize_with_memory(Addr mem, ShadowMap* PM);
void shadow_word_initialize_with_mmap(ShadowMap* PM);
void shadow_word_destroy_map(ShadowMap* PM);

#endif // shadow_32_word_h__

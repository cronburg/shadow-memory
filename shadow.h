/**
 * API / operations to perform on a shadow memory map  (based on memcheck)
 **/

#ifndef shadow_h__
#define shadow_h__

#include <stddef.h>
#include <sys/types.h> // TODO: remove
#include <sys/stat.h>  // TODO: remove
#include <fcntl.h>     // TODO: remove

// The possible states of memory:
#define DATA_UNDEF    0x0 // 00 binary
#define DATA_META     0x1 // 01 binary
//#define DATA_OBJECT     0x2 // 10 binary

#define DATA_UNDEF_8  0x00 // 0000 binary
#define DATA_META_8   0xff // 1111 binary
//#define DATA_OBJECT_8     0x2 // 10 binary

typedef unsigned char U8; //UChar;
typedef int Addr;
typedef struct {
  // granularity == (shadow_bits / application_bits) e.g. 1 shadow bit per 8 bits of
  //   application memory is all that is required for determining addressability
  //   (because memory is byte-addressed)
  short shadow_bits;        // # of shadow bits corresponding to one map entry
  short application_bits;   // # of app bits corresponding to one map entry
  void* map;                // pointer to the primary shadow map
  void* distinguished_maps; // pointer to distinguished maps
  short num_distinguished;  // # of distinguished maps
} ShadowMap;

// Two primary shadow map operations (get and set)
void shadow_get_meta_bits(ShadowMap* PM, Addr a, U8* mbits);
void shadow_set_meta_bits(ShadowMap* PM, Addr a, U8  mbits);

// Initialize and destroy. Initialize sets up the primary map and any distinguished maps.
// Destroy frees any memory malloc'd as part of the maps.
void shadow_initialize_map(ShadowMap* PM);
void shadow_destroy_map(ShadowMap* PM);

void snapshot(ShadowMap* PM);

// Application needs to explicitly determine how system calls are made
extern void  free(void* addr);
extern void* shadow_malloc(size_t size);
extern void* shadow_calloc(size_t nmemb, size_t size);
extern void  out_of_memory();
extern void  mem_copy(void* dst, void* src, size_t size);

#endif // shadow_h__

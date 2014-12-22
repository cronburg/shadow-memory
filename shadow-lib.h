/**
 * API / operations to perform on a shadow memory map  (based on memcheck)
 **/

#ifndef shadow_lib_h__
#define shadow_lib_h__

// The possible states of memory:
#define DATA_UNDEF    0x0 // 00 binary
#define DATA_META     0x1 // 01 binary
//#define DATA_OBJECT     0x2 // 10 binary

#define DATA_UNDEF_8  0x00 // 0000 binary
#define DATA_META_8   0xff // 1111 binary
//#define DATA_OBJECT_8     0x2 // 10 binary

typedef unsigned char U8; //UChar;
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

// Two primary shadow map operations (get and set a byte)
void shadow_get_bits(ShadowMap* PM, Addr a, U8* mbits);
void shadow_set_bits(ShadowMap* PM, Addr a, U8  mbits);

// set, unset, and get individual bits:
void shadow_mark_bit   (ShadowMap* PM, Addr a, U8 offset);
void shadow_unmark_bit (ShadowMap* PM, Addr a, U8 offset);
void shadow_get_bit    (ShadowMap* PM, Addr a, U8 offset, U8* bit);

// Initialize and destroy. Initialize sets up the primary map and any distinguished maps.
// Destroy frees any memory malloc'd as part of the maps.
void shadow_initialize_map(ShadowMap* PM);
void shadow_destroy_map(ShadowMap* PM);

// Application needs to explicitly determine how system calls are made
void  shadow_free(void* addr);
void* shadow_malloc(SizeT size);
void* shadow_calloc(SizeT nmemb, SizeT size);
void  shadow_out_of_memory(void);
void  shadow_memcpy(void* dst, void* src, SizeT size);

#endif // shadow_lib_h__

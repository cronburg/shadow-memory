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

#include "shadow_private.h"

// ----------------------------------------------------------------------------
INLINE
Int is_DSM(ShadowMap* PM, SM* sm) {
  return (sm >= (DMAP(PM)[0]) &&
          sm <= (DMAP(PM)[PM->num_distinguished - 1]));
}

INLINE
SM* copy_for_writing(SM* dist_SM) {
  SM* new_SM = (SM*)shadow_malloc(sizeof(SM));
  if (new_SM == NULL)
    shadow_out_of_memory();
  shadow_memcpy((void*)new_SM, (void*)dist_SM, sizeof(SM));
  return new_SM;
}

INLINE
SM* get_SM_for_reading(ShadowMap *PM, Addr a) {
  return MAP(PM)[a >> 16];
}

INLINE
SM* get_SM_for_writing(ShadowMap *PM, Addr a) {
  //SM** sm_p = &PM[a >> 16]; // bits [31..16]
  SM** sm_p = &(MAP(PM)[a >> 16]); //& 0xffff0000]); // bits [31..16]
  if (is_DSM(PM, *sm_p))
    *sm_p = copy_for_writing(*sm_p);
  return *sm_p;
}

// ----------------------------------------------------------------------------
// Public getters and setters

INLINE
void shadow_get_bits(ShadowMap *PM, Addr a, U8* mbits) {
  SM* sm = get_SM_for_reading(PM, a);
  *mbits = sm->mbits[a & 0x0000ffff];
}

INLINE
void shadow_set_bits(ShadowMap *PM, Addr a, U8  mbits) {
  SM* sm = get_SM_for_writing(PM, a);
  sm->mbits[a & 0x0000ffff] = mbits;
}

INLINE
void shadow_mark_bit(ShadowMap *PM, Addr a, U8 offset) {
  SM* sm = get_SM_for_writing(PM, a);
  sm->mbits[a & 0x0000ffff] |= (1 << offset);
}

INLINE
void shadow_unmark_bit(ShadowMap *PM, Addr a, U8 offset) {
  SM* sm = get_SM_for_writing(PM, a);
  sm->mbits[a & 0x0000ffff] &= ~(1 << offset);
}

INLINE
void shadow_get_bit(ShadowMap *PM, Addr a, U8 offset, U8* bit) {
  SM* sm = get_SM_for_writing(PM, a);
  *bit = (sm->mbits[a & 0x0000ffff] & (1 << offset)) >> offset;
}

// ----------------------------------------------------------------------------

INLINE
void shadow_initialize_map(ShadowMap* PM) {
  Int i;
  PM->distinguished_maps = shadow_calloc(NDIST(PM), sizeof(SM*)); // allocate array of distinguished maps
  PM->map = shadow_malloc(KB_64 * sizeof(SM*));
  SM* dist_maps = (SM*)shadow_calloc(sizeof(SM), NDIST(PM));
  for (i = 0; i < NDIST(PM); i++) {
    DMAP(PM)[i] = &(dist_maps[i]); // allocate each distinguished map
  }
  for (i = 0; i < KB_64; i++) {
    MAP(PM)[i] = DMAP(PM)[0];
  }
}

INLINE
void shadow_destroy_map(ShadowMap* PM) {
  Int i;
  for (i = 0; i < KB_64; i++) {
    if (! is_DSM(PM, MAP(PM)[i]))
      shadow_free(MAP(PM)[i]); // free all allocated (non-distinguished) maps)
  }
  for (i = 0; i < NDIST(PM); i++) {
    shadow_free(DMAP(PM)[i]);
  }
  shadow_free(MAP(PM));  // free the map array
  shadow_free(DMAP(PM)); // free the distinguished map array
}


#endif // shadow_lib_h__

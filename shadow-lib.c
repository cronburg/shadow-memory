#include "shadow_private.h"

// Primary Map is an array of pointers to Secondary Maps, e.g.:
//SM* PM[KB_64];

// Only one distinguished secondary map for now - UNDEFINED, e.g.:
//SM distinguished_SM;

INLINE
Int is_DSM(ShadowMap* PM, SM* sm) {
  return (sm >= (DMAP(PM)[0]) &&
          sm <= (DMAP(PM)[PM->num_distinguished - 1]));
}

INLINE
SM* copy_for_writing(SM* dist_SM) {
  SM* new_SM = (void*)shadow_malloc(sizeof(SM));
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
  for (i = 0; i < NDIST(PM); i++) {
    DMAP(PM)[i] = shadow_calloc(sizeof(SM), 1); // allocate each distinguished map
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


#include "shadow_private.h"

// Primary Map is an array of pointers to Secondary Maps, e.g.:
//SM* PM[KB_64];

// Only one distinguished secondary map for now - UNDEFINED, e.g.:
//SM distinguished_SM;

INLINE
int is_DSM(ShadowMap* PM, SM* sm) {
  return (sm >= (((SM**)(PM->distinguished_maps))[0]) &&
          sm <= (((SM**)(PM->distinguished_maps))[PM->num_distinguished - 1]));
}

INLINE
SM* copy_for_writing(SM* dist_SM) {
  SM* new_SM = (void*)shadow_malloc(sizeof(SM));
  if (new_SM == NULL)
    out_of_memory();
  mem_copy((void*)new_SM, (void*)dist_SM, sizeof(SM));
  return new_SM;
}

INLINE
SM* get_SM_for_reading(ShadowMap *PM, Addr a) {
  return ((SM**)PM->map)[a >> 16];
}

INLINE
SM* get_SM_for_writing(ShadowMap *PM, Addr a) {
  //SM** sm_p = &PM[a >> 16]; // bits [31..16]
  SM** sm_p = &(((SM**)PM->map)[a & 0xffff0000]); // bits [31..16]
  if (is_DSM(PM, *sm_p))
    *sm_p = copy_for_writing(*sm_p);
  return *sm_p;
}

INLINE
void shadow_get_meta_bits(ShadowMap *PM, Addr a, U8* mbits) {
  SM* sm = get_SM_for_reading(PM, a);
  *mbits = sm->mbits[a & 0x0000ffff];
}

INLINE
void shadow_set_meta_bits(ShadowMap *PM, Addr a, U8  mbits) {
  SM* sm = get_SM_for_writing(PM, a);
  sm->mbits[a & 0x0000ffff] = mbits;
}

INLINE
void shadow_initialize_map(ShadowMap* PM) {

}

INLINE
void shadow_destroy_map(ShadowMap* PM) {

}


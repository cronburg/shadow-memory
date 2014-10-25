#include "shadow.h"

// Primary Map is an array of pointers to Secondary Maps
SM* PM[KB_64];

// Only one distinguished secondary map for now - UNDEFINED
static SM distinguished_SM;

int INLINE is_DSM(SM *sm) {
  return (sm == &distinguished_SM);
}

SM* copy_for_writing(SM* dist_SM) {
  SM* new_SM = (void*)shadow_malloc(sizeof(SM));
  if (new_SM == NULL)
    out_of_memory();
  mem_copy((void*)new_SM, (void*)dist_SM, sizeof(SM));
  return new_SM;
}

SM* get_SM_for_reading(Addr a) {
  return PM[a >> 16];
}

SM* get_SM_for_writing(Addr a) {
  //SM** sm_p = &PM[a >> 16]; // bits [31..16]
  SM** sm_p = &PM[a & 0xffff0000]; // bits [31..16]
  if (is_DSM(*sm_p))
    *sm_p = copy_for_writing(*sm_p);
  return *sm_p;
}

void get_meta_bits(Addr a, U8* mbits) {
  SM* sm = get_SM_for_reading(a);
  *mbits = sm->mbits[a & 0x0000ffff];
}

void set_meta_bits(Addr a, U8  mbits) {
  SM* sm = get_SM_for_writing(a);
  sm->mbits[a & 0x0000ffff] = mbits;
}


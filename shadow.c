#include "shadow_private.h"

// Primary Map is an array of pointers to Secondary Maps, e.g.:
//SM* PM[KB_64];

// Only one distinguished secondary map for now - UNDEFINED, e.g.:
//SM distinguished_SM;

INLINE
int is_DSM(ShadowMap* PM, SM* sm) {
  return (sm >= (DMAP(PM)[0]) &&
          sm <= (DMAP(PM)[PM->num_distinguished - 1]));
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
  return MAP(PM)[a >> 16];
}

INLINE
SM* get_SM_for_writing(ShadowMap *PM, Addr a) {
  //SM** sm_p = &PM[a >> 16]; // bits [31..16]
  SM** sm_p = &(MAP(PM)[a & 0xffff0000]); // bits [31..16]
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
  int i;
  PM->distinguished_maps = shadow_calloc(NDIST(PM), sizeof(SM*)); // allocate array of distinguished maps
  PM->map = shadow_malloc(KB_64 * sizeof(SM*));
  for (i = 0; i < NDIST(PM); i++) {
    DMAP(PM)[i] = shadow_calloc(1, sizeof(SM)); // allocate each distinguished map
  }
  for (i = 0; i < KB_64; i++) {
    //printf("curr=0x%p, dmap[0]=0x%p, sizeof(U8)=%d\n", &MAP(PM)[i], DMAP(PM)[0],
    //       (int)sizeof(U8));
    MAP(PM)[i] = DMAP(PM)[0];
    //printf("%p, %p\n", &((SM**)(PM->map))[i], DMAP(PM)[0]);
    //SM* lvalue = &(((SM**)(PM->map))[i]);
    //*lvalue = DMAP(PM)[0];
    //MAP(PM)[i] = DMAP(PM)[0]; // TODO: figure out which distinguished map to use
  }
}

INLINE
void shadow_destroy_map(ShadowMap* PM) {
  int i;
  for (i = 0; i < KB_64; i++) {
    if (! is_DSM(PM, MAP(PM)[i]))
      free(MAP(PM)[i]); // free all allocated (non-distinguished) maps)
  }
  for (i = 0; i < NDIST(PM); i++) {
    free(DMAP(PM)[i]);
  }
  free(MAP(PM));  // free the map array
  free(DMAP(PM)); // free the distinguished map array
}


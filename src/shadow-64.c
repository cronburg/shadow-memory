#include "shadow-64.h"

INLINE
Low* copy_for_writing(Low* dist_Low) {
  Low* new_Low = (Low*)shadow_malloc(sizeof(Low));
  if (new_Low == NULL)
    shadow_out_of_memory();
  shadow_memcpy((void*)new_Low, (void*)dist_Low, sizeof(Low));
  return new_Low;
}

INLINE
Low* get_Low_for_reading(ShadowMap *PM, Addr a) {
  SizeT idx = a >> 35;
  assert(idx < 536870912);
  MiddleLowPtr ml = BITS(PM)[idx];
  if (ml.l == DMAP(PM))
    return ml.l;
  idx = (a & MIDDLE_BITS) >> 18;
  assert(idx < 131072);
  return (ml.m->bits)[idx];
}

INLINE
Low* get_Low_for_writing(ShadowMap *PM, Addr a) {
  MiddleLowPtr* ml = &(BITS(PM)[a >> 35]);
  if (ml->l == DMAP(PM)) {
    ml->m = (Middle*)shadow_malloc(sizeof(Middle));
    memcpy(ml->m, PM->distinguished_middle, sizeof(Middle));
  }
  SizeT idx = (a & MIDDLE_BITS) >> 18;
  assert(idx < 131072);
  Low** low = &(ml->m->bits[idx]);
  if (*low == DMAP(PM))
    *low = copy_for_writing(*low);
  return *low;
}

INLINE
void shadow_get_bits(ShadowMap *PM, Addr a, U8* mbits) {
  Low* low = get_Low_for_reading(PM, a);
  SizeT idx = a & LOW_BITS;
  assert(idx < 262144);
  *mbits = low->bits[a & LOW_BITS];
}

INLINE
void shadow_set_bits(ShadowMap *PM, Addr a, U8  mbits) {
  Low* low = get_Low_for_writing(PM, a);
  SizeT idx = a & LOW_BITS;
  assert(idx < 262144);
  low->bits[idx] = mbits;
}

INLINE
void shadow_mark_bit(ShadowMap *PM, Addr a, U8 offset) {
  Low* low = get_Low_for_writing(PM, a);
  SizeT idx = a & LOW_BITS;
  assert(idx < 262144);
  low->bits[idx] |= (1 << offset);
}

INLINE
void shadow_unmark_bit(ShadowMap *PM, Addr a, U8 offset) {
  Low* low = get_Low_for_writing(PM, a);
  SizeT idx = a & LOW_BITS;
  assert(idx < 262144);
  low->bits[idx] &= ~(1 << offset);
}

INLINE
void shadow_get_bit(ShadowMap *PM, Addr a, U8 offset, U8* bit) {
  Low* low = get_Low_for_writing(PM, a);
  SizeT idx = a & LOW_BITS;
  assert(idx < 262144);
  *bit = (low->bits[idx] & (1 << offset)) >> offset;
}

INLINE
void shadow_initialize_map(ShadowMap* PM) {
  SizeT i;

  // mmap allocate virtual space of the primary map.
  //PM->map = mmap(NULL, HIGH_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  if (PM->map == MAP_FAILED) {
    perror("Failed to allocate primary map.");
    return;
  }

  // Allocate the distinguished low map and have all entries of the primary map
  // point to it by memcpy-ing the contents of an array pointing to it.
  //DMAP(PM) = (Low*) shadow_calloc(1, LOW_SIZE);
  Low* src[4096];
  for (i = 0; i < 4096; i++) {
    src[i] = DMAP(PM);
  }
  // units(i) = bytes, units(memcpy) = bytes
  for (i = 0; i < HIGH_SIZE; i += 4096 * 8) {
    memcpy((char*)(& BITS(PM)) + i, (void*)src, 4096 * 8);
  }

  // Set the distinguished middle map to have all entries pointing to the
  // (single) distinguished low map.
  //PM->distinguished_middle = (Middle*) shadow_malloc(MID_SIZE);
  for (i = 0; i < 131072; i++) {
    PM->distinguished_middle->bits[i] = DMAP(PM);
  }
}

INLINE
void shadow_initialize_with_memory(Addr mem, ShadowMap* PM) {
  // TODO: assert good alignment
  PM->map = (High*)mem;
  DMAP(PM) = (Low*)(((U8*)mem) + HIGH_SIZE);
  memset(DMAP(PM), 0, LOW_SIZE);
  PM->distinguished_middle = (Middle*)(((U8*)mem) + HIGH_SIZE + LOW_SIZE);
  shadow_initialize_map(PM);
}

INLINE
void shadow_initialize_with_mmap(ShadowMap* PM) {
  PM->map = (High*)mmap(NULL, HIGH_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  DMAP(PM) = (Low*) shadow_calloc(1, LOW_SIZE);
  PM->distinguished_middle = (Middle*) shadow_malloc(MID_SIZE);
  shadow_initialize_map(PM);
}

INLINE
void shadow_destroy_map(ShadowMap* PM) {

}


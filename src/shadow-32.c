#include "shadow-32.h"

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
  SizeT idx = a >> NUM_LOW_BITS;
  assert(idx < HIGH_COUNT);
  Low* l = BITS(PM)[idx];
  return l;
}

INLINE
Low* get_Low_for_writing(ShadowMap *PM, Addr a) {
  SizeT idx = a >> NUM_LOW_BITS;
  assert(idx < HIGH_COUNT);
  Low** low = &(BITS(PM)[idx]);
  if (*low == DMAP(PM))
    *low = copy_for_writing(*low);
  return *low;
}

INLINE
void shadow_get_bits(ShadowMap *PM, Addr a, U8* mbits) {
  Low* low = get_Low_for_reading(PM, a);
  SizeT idx = a & LOW_BITS;
  assert(idx < LOW_COUNT);
  *mbits = low->bits[idx];
}

INLINE
void shadow_set_bits(ShadowMap *PM, Addr a, U8  mbits) {
  Low* low = get_Low_for_writing(PM, a);
  SizeT idx = a & LOW_BITS;
  assert(idx < LOW_COUNT);
  low->bits[idx] = mbits;
}

INLINE
void shadow_mark_bit(ShadowMap *PM, Addr a, U8 offset) {
  Low* low = get_Low_for_writing(PM, a);
  SizeT idx = a & LOW_BITS;
  assert(idx < LOW_COUNT);
  low->bits[idx] |= (1 << offset);
}

INLINE
void shadow_unmark_bit(ShadowMap *PM, Addr a, U8 offset) {
  Low* low = get_Low_for_writing(PM, a);
  SizeT idx = a & LOW_BITS;
  assert(idx < LOW_COUNT);
  low->bits[idx] &= ~(1 << offset);
}

INLINE
void shadow_get_bit(ShadowMap *PM, Addr a, U8 offset, U8* bit) {
  Low* low = get_Low_for_writing(PM, a);
  SizeT idx = a & LOW_BITS;
  assert(idx < LOW_COUNT);
  *bit = (low->bits[idx] & (1 << offset)) >> offset;
}

INLINE
void shadow_initialize_map(ShadowMap* PM) {
  SizeT i;

  // mmap allocate virtual space of the primary map.
  //PM->map = mmap(NULL, HIGH_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  if (PM->map == MAP_FAILED) {
    perror("Failed to allocate primary map");
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
  for (i = 0; i < HIGH_SIZE; i += 4096 * sizeof(Low*)) {
    memcpy((char*)(& BITS(PM)) + i, (void*)src, 4096 * sizeof(Low*));
  }

}

INLINE
void shadow_initialize_with_memory(Addr mem, ShadowMap* PM) {
  // TODO: assert good alignment
  PM->map = (High*)mem;
  DMAP(PM) = (Low*)(((U8*)mem) + HIGH_SIZE);
  memset(DMAP(PM), 0, LOW_SIZE);
  shadow_initialize_map(PM);
}

INLINE
void shadow_initialize_with_mmap(ShadowMap* PM) {
  PM->map = (High*)mmap(NULL, HIGH_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  DMAP(PM) = (Low*) shadow_calloc(1, LOW_SIZE);
  shadow_initialize_map(PM);
}

INLINE
void shadow_destroy_map(ShadowMap* PM) {

}


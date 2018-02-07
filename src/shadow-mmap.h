#ifndef shadow_mmap_h__
#define shadow_mmap_h__

// >>> hex(2**35 - 1)
//#define LOW_HALF_BITS 0x00000007ffffffff

// >>> hex(2**18 - 1)
#define LOW_BITS 0x3ffff
typedef struct { U8 bits[262144]; } Low; // 2**18 bytes (bit for bit shadow map)

// >>> hex(2**(17+18) - 1 - (2**18 - 1))
#define MIDDLE_BITS 0x7fffc0000
typedef struct { Low* bits[131072]; } Middle; // 2**17 pointers

typedef union {
  Middle* m;
  Low* l;
} MiddleLowPtr;

// >>> hex(2**64 - 2**(17+18))
#define HIGH_BITS 0xfffffff800000000
typedef struct { MiddleLowPtr bits[536870912]; } High; // 2**29 pointers

// >>> hex(0xfffffff800000000 + 0x7fffc0000 + 0x3ffff)
// '0xffffffffffffffff'

typedef struct {
  High* map;              // pointer to the primary shadow map
  Low* distinguished_map; // pointer to the (only) distinguished map
  Middle* distinguished_middle;
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
Low* copy_for_writing(Low* low);

// Secondary Map getters (maps application address to which Low that address is in)
Low* get_Low_for_reading(ShadowMap *PM, Addr a);
Low* get_Low_for_writing(ShadowMap *PM, Addr a);

// ----------------------------------------------------------------------------

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

#define HIGH_SIZE 4294967296
#define LOW_SIZE (sizeof(Low))
#define MID_SIZE (sizeof(Middle))
#define SHADOW_INIT_SIZE (HIGH_SIZE + LOW_SIZE + MID_SIZE)

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
    memcpy((void*)(& BITS(PM)) + i, (void*)src, 4096 * 8);
  }

  // Set the distinguished middle map to have all entries pointing to the
  // (single) distinguished low map.
  //PM->distinguished_middle = (Middle*) shadow_malloc(MID_SIZE);
  for (i = 0; i < 131072; i++) {
    PM->distinguished_middle->bits[i] = DMAP(PM);
  }
}

INLINE
void shadow_initialize_with_memory(void* mem, ShadowMap* PM) {
  // TODO: assert good alignment
  PM->map = mem;
  DMAP(PM) = (Low*)(((char*)mem) + HIGH_SIZE);
  memset(DMAP(PM), 0, LOW_SIZE);
  PM->distinguished_middle = (Middle*)(((char*)mem) + HIGH_SIZE + LOW_SIZE);
  shadow_initialize_map(PM);
}

INLINE
void shadow_initialize_with_mmap(ShadowMap* PM) {
  PM->map = mmap(NULL, HIGH_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  DMAP(PM) = (Low*) shadow_calloc(1, LOW_SIZE);
  PM->distinguished_middle = (Middle*) shadow_malloc(MID_SIZE);
  shadow_initialize_map(PM);
}

INLINE
void shadow_destroy_map(ShadowMap* PM) {

}

#endif // shadow_mmap_h__

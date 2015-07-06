/**
 * Wrapper for 32-bit address space values in shadow memory maps
 **/
#ifndef shadow_lib_32_h__
#define shadow_lib_32_h__
#include "shadow-lib.h"

typedef long unsigned int U32;

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define get_mbits32(n) (&(((U8*)mbits)[3 - n]))
#define set_mbits32(n) (((U8*)(&mbits))[3 - n])
#else
#define get_mbits32(n) (&(((U8*)mbits)[n]))
#define set_mbits32(n) (((U8*)(&mbits))[n])
#endif

#define map32(n) (&(((ShadowMap*) (PM->map))[n]))

INLINE
void shadow32_get_bits(ShadowMap* PM, Addr a, U32* mbits) {
  shadow_get_bits(map32(0), a, get_mbits32(0));
  shadow_get_bits(map32(1), a, get_mbits32(1));
  shadow_get_bits(map32(2), a, get_mbits32(2));
  shadow_get_bits(map32(3), a, get_mbits32(3));
}

INLINE
void shadow32_set_bits(ShadowMap* PM, Addr a, U32 mbits) {
  shadow_set_bits(map32(0), a, set_mbits32(0));
  shadow_set_bits(map32(1), a, set_mbits32(1));
  shadow_set_bits(map32(2), a, set_mbits32(2));
  shadow_set_bits(map32(3), a, set_mbits32(3));
}

#define __shadow32_bit_op(fncn) \
  if        (offset < 8) { \
    fncn(map32(3), a, offset); \
  } else if (offset < 16) { \
    fncn(map32(2), a, offset - 8); \
  } else if (offset < 24)  { \
    fncn(map32(1), a, offset - 16); \
  } else { \
    fncn(map32(0), a, offset - 24); \
  }

// Note that upper 3 bits of offset are ignored (offset is really U5)
INLINE
void shadow32_mark_bit   (ShadowMap* PM, Addr a, U8 offset) {
  __shadow32_bit_op(shadow_mark_bit)
}

INLINE
void shadow32_unmark_bit (ShadowMap* PM, Addr a, U8 offset) {
  __shadow32_bit_op(shadow_unmark_bit);
}

INLINE
void shadow32_get_bit    (ShadowMap* PM, Addr a, U8 offset, U32* mbits) {
  if        (offset < 8) {
    shadow_get_bit(map32(3), a, offset, get_mbits32(0));
  } else if (offset < 16) {
    shadow_get_bit(map32(2), a, offset - 8, get_mbits32(1));
  } else if (offset < 24)  {
    shadow_get_bit(map32(1), a, offset - 16, get_mbits32(2));
  } else {
    shadow_get_bit(map32(0), a, offset - 24, get_mbits32(3));
  }

}

#define __shadow32_init(n) \
  map32(n)->shadow_bits = 1; \
  map32(n)->application_bits = 1; \
  map32(n)->num_distinguished = 1; \
  shadow_initialize_map(map32(n))

INLINE
void shadow32_initialize_map(ShadowMap* PM) {
  PM->map = (void*) shadow_calloc(4, sizeof(ShadowMap));
  __shadow32_init(0);
  __shadow32_init(1);
  __shadow32_init(2);
  __shadow32_init(3);
}

INLINE
void shadow32_destroy_map   (ShadowMap* PM) {
  shadow_destroy_map(map32(0));
  shadow_destroy_map(map32(1));
  shadow_destroy_map(map32(2));
  shadow_destroy_map(map32(3));
  shadow_free(PM->map);
}

#endif // shadow_lib_32_h__


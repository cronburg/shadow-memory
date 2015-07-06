/**
 * Wrapper for 32-bit address space values in shadow memory maps
 **/
#ifndef shadow_lib_32_h__
#define shadow_lib_32_h__
#include "shadow-lib.h"

void shadow32_get_bits(ShadowMap* PM, Addr a, U32* mbits);
void shadow32_set_bits(ShadowMap* PM, Addr a, U32  mbits);
void shadow32_mark_bit   (ShadowMap* PM, Addr a, U8 offset);
void shadow32_unmark_bit (ShadowMap* PM, Addr a, U8 offset);
void shadow32_get_bit    (ShadowMap* PM, Addr a, U8 offset, U8* bit);
void shadow32_initialize_map(ShadowMap* PM);
void shadow32_destroy_map   (ShadowMap* PM);

#endif // shadow_lib_32_h__


/**
 * API / operations to perform on a shadow memory map  (based on memcheck)
 **/

#ifndef shadow_private_h__
#define shadow_private_h__

#include <stddef.h>
#include "shadow.h"

#define KB_64        65536
#define INLINE    inline __attribute__((always_inline))

// Secondary Map structure
typedef struct {
  U8 mbits[KB_64];
} SM;

// Whether or not the given SM is the "distinguished" secondary map
// UNDEFINED DSM is what all PM entries point to initially before any
// memory accesses occur. 
int shadow_is_DSM(SM* sm);

// Allocates and initializes a new SM
SM* shadow_copy_for_writing(ShadowMap *PM, SM* sm);

// Secondary Map getters (maps application address to which SM that address is in)
SM* get_SM_for_reading(ShadowMap *PM, Addr a);
SM* get_SM_for_writing(ShadowMap *PM, Addr a);

#endif // shadow_private_h__

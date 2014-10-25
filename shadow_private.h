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

// Macros for accessing map fields of primary shadow map struct:
#define MAP(PM)   ((SM**)(PM->map))
#define DMAP(PM)  ((SM**)(PM->distinguished_maps))
#define NDIST(PM) (PM->num_distinguished)

// Whether or not the given SM is the "distinguished" secondary map
// UNDEFINED DSM is what all PM entries point to initially before any
// memory accesses occur. 
int is_DSM(ShadowMap *PM, SM* sm);

// Allocates and initializes a new SM
SM* copy_for_writing(SM* sm);

// Secondary Map getters (maps application address to which SM that address is in)
SM* get_SM_for_reading(ShadowMap *PM, Addr a);
SM* get_SM_for_writing(ShadowMap *PM, Addr a);

#endif // shadow_private_h__

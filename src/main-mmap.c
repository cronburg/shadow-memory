#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// ----------------------------------------------------------------------------
// The following might end up in its own header file eventually, but for now
// only the application can really know how to set the right types and system
// calls.
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

typedef int Int;
typedef size_t SizeT;
typedef unsigned long int Addr;
typedef unsigned char U8; //UChar;

void  shadow_free(void* addr) { free(addr); }
void *shadow_malloc(size_t size) { return malloc(size); }
void *shadow_calloc(size_t nmemb, size_t size) { return calloc(nmemb, size); }
void  shadow_memcpy(void* dst, void* src, size_t size) { memcpy(dst,src,size); }
void  shadow_out_of_memory() {
  printf("ERROR: Ran out of memory while allocating shadow memory.\n");
  exit(1);
}
// ----------------------------------------------------------------------------

#include "shadow-mmap.h"

#define print(u8) printf("tmp = 0x%x\n", u8)
#define getbyte(addr,u8) shadow_get_bits(my_sm, addr, &u8)
#define getbit(addr,offset,u8)  shadow_get_bit (my_sm, addr, offset, &u8)

int main(int argc, char *argv[]) {
  printf("Entering main()\n");

  ShadowMap* my_sm = malloc(sizeof(ShadowMap));
  shadow_initialize_with_mmap(my_sm);
  U8 tmp;
  
  shadow_set_bits(my_sm, 0, (U8)0xab);
  getbyte(0,tmp); print(tmp); assert(tmp == (U8)0xab);

  shadow_set_bits(my_sm,   0x1111, (U8)0xFF);
  shadow_unmark_bit(my_sm, 0x1111, 4);
  getbyte(0x1111,tmp); print(tmp); assert(tmp == (U8)0xEF); // TODO: assert
  getbit (0x1111,4,tmp); assert(tmp == 0);

  shadow_set_bits(my_sm, 0xBEEF, (U8)0x00);
  shadow_mark_bit(my_sm, 0xBEEF, 2);
  getbyte(0xBEEF,tmp); print(tmp); assert(tmp == (U8)0x04);
  getbit (0xBEEF,2,tmp); assert(tmp == 1);
  
  printf("----\n");
  shadow_set_bits(my_sm, 0xAAFF00AAFFFF0000, (U8)0x00);
  shadow_mark_bit(my_sm, 0xAAFF00AAFFFF0000, 2);
  getbyte(0xAAFF00AAFFFF0000,tmp); print(tmp); assert(tmp == (U8)0x04);
  getbit (0xAAFF00AAFFFF0000,2,tmp); assert(tmp == 1);

  shadow_destroy_map(my_sm);
  free(my_sm);
  printf("Exiting main()\n");
  return 0;
}


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

void  shadow_free(void* addr) { free(addr); }
void *shadow_malloc(size_t size) { return malloc(size); }
void *shadow_calloc(size_t nmemb, size_t size) { return calloc(nmemb, size); }
void  shadow_memcpy(void* dst, void* src, size_t size) { memcpy(dst,src,size); }
void  shadow_out_of_memory() {
  printf("ERROR: Ran out of memory while allocating shadow memory.\n");
  exit(1);
}
// ----------------------------------------------------------------------------

#include "shadow-32-word.h"

#define print(u32) printf("tmp = %p\n", (void*)u32)
#define getbyte(addr,u32) shadow_word_get_bits(my_sm, addr, &u32)
#define getbit(addr,offset,u32)  shadow_word_get_bit (my_sm, addr, offset, &u32)

int main(int argc, char *argv[]) {
  printf("Entering main()\n");

  ShadowMap* my_sm = malloc(sizeof(ShadowMap));
  shadow_word_initialize_with_mmap(my_sm);
  U32 tmp;
  
  shadow_word_set_bits(my_sm, 0, (U32)0xabcdef01);
  getbyte(0,tmp); print(tmp); assert(tmp == (U32)0xabcdef01);

  shadow_word_set_bits(my_sm,   0x1111, (U32)0xFF);
  shadow_word_unmark_bit(my_sm, 0x1111, 4);
  getbyte(0x1111,tmp); print(tmp); assert(tmp == (U32)0xEF); // TODO: assert
  getbit (0x1111,4,tmp); assert(tmp == 0);

  shadow_word_set_bits(my_sm, 0xBEEF, (U32)0x00);
  shadow_word_mark_bit(my_sm, 0xBEEF, 2);
  getbyte(0xBEEF,tmp); print(tmp); assert(tmp == (U32)0x04);
  getbit (0xBEEF,2,tmp); assert(tmp == 1);
  
  printf("----\n");
  shadow_word_set_bits(my_sm, 0xAAFF00AA, (U32)0x00);
  shadow_word_mark_bit(my_sm, 0xAAFF00AA, 2);
  getbyte(0xAAFF00AA,tmp); print(tmp); assert(tmp == (U32)0x04);
  getbit (0xAAFF00AA,2,tmp); assert(tmp == 1);

  shadow_word_destroy_map(my_sm);
  free(my_sm);
  printf("Exiting main()\n");
  return 0;
}


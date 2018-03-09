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

#include "shadow-64-word.h"

#define print(u64) printf("tmp = %p\n", (void*)u64)
#define getbyte(addr,u64) shadow_word_get_bits(my_sm, addr, &u64)
#define getbit(addr,offset,u64)  shadow_word_get_bit (my_sm, addr, offset, &u64)

int main(int argc, char *argv[]) {
  printf("Entering main()\n");

  ShadowMap* my_sm = malloc(sizeof(ShadowMap));
  shadow_word_initialize_with_mmap(my_sm);
  U64 tmp;
  U8 tmp2;
  
  shadow_word_set_bits(my_sm, 0, (U64)0xabcdef01abcdef01);
  getbyte(0,tmp); print(tmp); assert(tmp == (U64)0xabcdef01abcdef01);

  shadow_word_set_bits(my_sm,   0x1111, (U64)0xFF);
  shadow_word_unmark_bit(my_sm, 0x1111, 4);
  getbyte(0x1111,tmp); print(tmp); assert(tmp == (U64)0xEF); // TODO: assert
  getbit (0x1111,4,tmp2); assert(tmp2 == 0);

  shadow_word_set_bits(my_sm, 0xBEEF, (U64)0x00);
  shadow_word_mark_bit(my_sm, 0xBEEF, 2);
  getbyte(0xBEEF,tmp); print(tmp); assert(tmp == (U64)0x04);
  getbit (0xBEEF,2,tmp2); assert(tmp2 == 1);
  
  printf("----\n");
  shadow_word_set_bits(my_sm, 0xAAFF00AAFFFFFFFF, (U64)0x00);
  shadow_word_mark_bit(my_sm, 0xAAFF00AAFFFFFFFF, 2);
  getbyte(0xAAFF00AAFFFFFFFF,tmp); print(tmp); assert(tmp == (U64)0x04);
  getbit (0xAAFF00AAFFFFFFFF,2,tmp2); assert(tmp2 == 1);

  shadow_word_destroy_map(my_sm);
  free(my_sm);
  printf("Exiting main()\n");
  return 0;
}


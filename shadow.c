#include "shadow.h"

#include "shadow-lib.c"

void  shadow_free(void* addr) { free(addr); }
void *shadow_malloc(size_t size) { return malloc(size); }
void *shadow_calloc(size_t nmemb, size_t size) { return calloc(nmemb, size); }
void  shadow_memcpy(void* dst, void* src, size_t size) { memcpy(dst,src,size); }
void  shadow_out_of_memory() {
  printf("ERROR: Ran out of memory while allocating shadow memory.\n");
  exit(1);
}


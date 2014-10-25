#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shadow.h"

void  shadow_free(void* addr) { free(addr); }
void *shadow_malloc(size_t size) { return malloc(size); }
void *shadow_calloc(size_t nmemb, size_t size) { return calloc(nmemb, size); }
void  mem_copy(void* dst, void* src, size_t size) { memcpy(dst,src,size); }
void  out_of_memory() {
  printf("ERROR: Ran out of memory while allocating shadow memory.\n");
  exit(1);
}


int main(int argc, char *argv[]) {
  printf("Entering main()\n");

  ShadowMap* my_sm = malloc(sizeof(ShadowMap));
  my_sm->shadow_bits = 1;
  my_sm->application_bits = 1;
  my_sm->num_distinguished = 1;
  shadow_initialize_map(my_sm);
  
  

  shadow_destroy_map(my_sm);
  free(my_sm);
  printf("Exiting main()\n");
  return 0;
}


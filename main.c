#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shadow.h"

void *shadow_malloc(size_t size) { return malloc(size); }
void  mem_copy(void* dst, void* src, size_t size) { memcpy(dst,src,size); }
void  out_of_memory() {
  printf("ERROR: Ran out of memory while allocating shadow memory.\n");
  exit(1);
}

int main(int argc, char *argv[]) {
  printf("Entering main()\n");
  copy_for_writing((void*)0);
  printf("Exiting main()\n");
  return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shadow.c"

int main(int argc, char *argv[]) {
  printf("Entering main()\n");

  ShadowMap* my_sm = malloc(sizeof(ShadowMap));
  my_sm->shadow_bits = 1;
  my_sm->application_bits = 1;
  my_sm->num_distinguished = 1;
  shadow_initialize_map(my_sm);
  
  shadow_set_meta_bits(my_sm, 0, (U8)0xab);
  U8 tmp;
  shadow_get_meta_bits(my_sm, 0, &tmp);
  printf("U8 tmp = %x\n", tmp);

  shadow_destroy_map(my_sm);
  free(my_sm);
  printf("Exiting main()\n");
  return 0;
}


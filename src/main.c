#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "shadow.h"

#define print(u8) printf("tmp = 0x%x\n", u8)
#define getbyte(addr,u8) shadow_get_bits(my_sm, addr, &u8)
#define getbit(addr,offset,u8)  shadow_get_bit (my_sm, addr, offset, &u8)

int main(int argc, char *argv[]) {
  printf("Entering main()\n");

  ShadowMap* my_sm = malloc(sizeof(ShadowMap));
  my_sm->shadow_bits = 1;
  my_sm->application_bits = 1;
  my_sm->num_distinguished = 1;
  shadow_initialize_map(my_sm);
  U8 tmp;
  
  shadow_set_bits(my_sm, 0, (U8)0xab);
  getbyte(0,tmp); print(tmp); assert(tmp == (U8)0xab);

  shadow_set_bits( my_sm,  0x1111, (U8)0xFF);
  shadow_unmark_bit(my_sm, 0x1111, 4);
  getbyte(0x1111,tmp); print(tmp); assert(tmp == (U8)0xEF); // TODO: assert
  getbit (0x1111,4,tmp); assert(tmp == 0);

  shadow_set_bits( my_sm, 0xBEEF, (U8)0x00);
  shadow_mark_bit(  my_sm, 0xBEEF, 2);
  getbyte(0xBEEF,tmp); print(tmp); assert(tmp == (U8)0x04);
  getbit (0xBEEF,2,tmp); assert(tmp == 1);

  shadow_destroy_map(my_sm);
  free(my_sm);
  printf("Exiting main()\n");
  return 0;
}


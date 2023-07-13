#include "../bitmap.h"
#include "../bitmap.c"
#include <stdio.h>
#include <stdlib.h>
/**
 File contenente test per la struttura dati bitmap

 Test su: inizializzazione bitmap, set e get, set errato
 
*/

void test_init() {
  uint8_t buffer[16];
  
  Bit_map bm;
  bit_map_init(&bm, buffer, 16);
  
  if(bm.data != buffer) {
    printf("Test init failed! Data does not match buffer\n");
    exit(1);
  }
  
  if(bm.dim != 16) {
    printf("Test init failed! Dim does not match buffer size\n");
    exit(1); 
  }
  
  if(bm.num_bit != 16 * 8) {
    printf("Test init failed! Num bit is not buffer size * 8\n");
    exit(1);
  }
  
  printf("Test init OK!\n");
}

void test_set_get() {
  uint8_t buffer[16];
  
  Bit_map bm;
  bit_map_init(&bm, buffer, 16);
  
  bit_map_set(&bm, 10, 1);
  if(bit_map_get(&bm, 10) != 1) {
    printf("Test set/get failed!\n");
    exit(1);
  }
  
  bit_map_set(&bm, 15, 0);
  if(bit_map_get(&bm, 15) != 0) {
    printf("Test set/get failed!\n");
    exit(1);
  }
  
  printf("Test set/get OK!\n");
}

void test_set_err() {
  uint8_t buffer[16];
  
  Bit_map bm;
  bit_map_init(&bm, buffer, 16);

  bit_map_set(&bm, 128, 1); // Bit 128 is out of range
  
  if(errno != EINVAL) {
    printf("Test set err failed! errno not set\n");
    exit(1);
  }
  
  printf("Test set err OK!\n");
}

void test_get_err() {
  uint8_t buffer[16];
  
  Bit_map bm; 
  bit_map_init(&bm, buffer, 16);

  bit_map_get(&bm, 128); // Bit 128 is out of range
  
  printf("Test get err OK!\n");
}


int main() {
  test_init();
  test_set_get();
  test_set_err();
  test_get_err();
  
  return 0;
}

#include "../bitmap.h"
#include "../bitmap.c"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/**
 File contenente test per la struttura dati bitmap
 Test su: inizializzazione bitmap, set e get, set errato
*/
int num_test = 2;
int contatore_test_giusti = 0;

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
  contatore_test_giusti++;
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
  contatore_test_giusti++;
  printf("Test set/get OK!\n");
}

int main() {
  test_init();
  test_set_get();

  
  printf("Test eseguiti correttamente: %d/%d\n", contatore_test_giusti, num_test);
  return 0;
}

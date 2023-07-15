#include "../bitmap.h"
#include "../bitmap.c"
#include "../Buddy_Allocator.h"
#include "../Buddy_Allocator.c"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#define BUDDY_SIZE 1048576 // 1 MB
#define NUM_LEVELS 10
#define MINIMUM_BLOCK_SIZE (BUDDY_SIZE >> (NUM_LEVELS - 1)) //2048 
#define NUMBER_OF_BUDDIES (1 << NUM_LEVELS) - 1
#define BITSET_SIZE (NUMBER_OF_BUDDIES + 7) >> 3 

/**
 * File contenente test per la struttura dati buddy allocator
 * Test su: inizializzazione buddy allocator, get_parent_index, from_index_to_level, get_left_child_index, get_right_child_index, offset_from_first, first_index_from_level, get_buddy_index, search_free_block
*/
int num_test = 6;
int num_success = 0;

void test_buddy_allocator() {
  // Creazione della bitmap
  Bit_map bitmap;
  uint8_t bitmap_buffer[BITSET_SIZE];
  //andrebbe inizializzato a 1, poiche tutti i blocchi sono liberi
  for(int i = 0; i < BITSET_SIZE; i++){
    bitmap_buffer[i] = 1;
  }
  printf("\n\nbitmap_size: %d\n", BITSET_SIZE);
  bit_map_init(&bitmap, bitmap_buffer, BITSET_SIZE);
  

  // Creazione dell'allocator di buddy
  Buddy_allocator buddy_allocator;
  uint8_t mem_buffer [BUDDY_SIZE];
  int num_levels = NUM_LEVELS;
  int minimum_block_size = MINIMUM_BLOCK_SIZE;
  buddy_allocator_init(&buddy_allocator, mem_buffer, &bitmap, num_levels, minimum_block_size);
  printf("num_levels: %d\n", num_levels);
  printf("minimum_block_size: %d", minimum_block_size);
 
  // Testing dell'allocazione e deallocazione di blocchi di memoria
  printf("\n\nTesting allocazione di blocco di dim 2048\n");
  int *ptr1 = buddy_allocator_alloc(&buddy_allocator, 2048);//valore troppo basso dato che il minimo è 2048 
  if(ptr1 == NULL){
    printf("ptr1: Errore nell'allocazione di blocco di memoria da 2048\n");
  }else{
    printf("ptr1: Allocazione riuscita di un blocco di dim 2048\n");
    num_success++;
    buddy_allocator_free_buddy(&buddy_allocator, ptr1);
  }
  printf("\n\nTesting allocazione di blocco di dim 4096\n");
  int *ptr2 = buddy_allocator_alloc(&buddy_allocator, 2 * 2048);
  if(ptr2 == NULL){
    printf("ptr2: Errore nell'allocazione di blocco di memoria da 4096\n");
  }else{
    printf("ptr2: Allocazione riuscita di un blocco di dim 4096\n");
    num_success++;
    buddy_allocator_free_buddy(&buddy_allocator, ptr2);
  }
  printf("\n\nTesting allocazione di blocco di dim 6144\n");
  int *ptr3 = buddy_allocator_alloc(&buddy_allocator, 3 * 2048);
  if(ptr3 == NULL){
    printf("ptr3: Errore nell'allocazione di blocco di memoria da 6144\n");
  }else{
    printf("ptr3: Allocazione riuscita di un blocco di dim 6144\n");
    num_success++;
    buddy_allocator_free_buddy(&buddy_allocator, ptr3);
  }
 
  // Testing di allocazione di blocchi di dimensione superiore alla dimensione massima
  printf("\n\nTesting di allocazione di blocchi di dimensione superiore alla dimensione massima\n");
  void *ptr7 = buddy_allocator_alloc(&buddy_allocator, BUDDY_SIZE);
  if (ptr7 != NULL) {
    printf("ptr7: ERRORE hai allocato un blocco di dimensioni superiori a quelle disponibili\n");
    buddy_allocator_free_buddy(&buddy_allocator, ptr7);
  } else {
    printf("ptr7: CORRETTO Non sei riusciuto ad allocare un blocco di dimensioni superiori a quelle disponibili\n");
    num_success++;
  }
  
  //TESTING DI ALLOCAZIONE DI DUE BLOCCHI LA CUI SOMMA SUPERA LA DIMENSIONE MASSIMA
  printf("\n\nTesting di allocazione di due blocchi la cui somma supera la dimensione massima\n");
  void *ptr8 = buddy_allocator_alloc(&buddy_allocator, 524288); 
  void *ptr9 = buddy_allocator_alloc(&buddy_allocator, 524288);
  if (ptr8 != NULL && ptr9 != NULL) {
    printf("ptr8, ptr9: ERRORE SEI RIUSCITO AD ALLOCARE DUE BLOCCHI LA CUI SOMMA SUPERA LA DIMENSIONE MASSIMA\n");
    buddy_allocator_free_buddy(&buddy_allocator, ptr8);
    buddy_allocator_free_buddy(&buddy_allocator, ptr9);
  }
  if(ptr8 == NULL && ptr9 == NULL){
    printf("ptr8, ptr9: SBAGLIATO NON SEI RIUSCITO AD ALLOCARE NESSUNO DEI DUE BLOCCHI, IL PRIMO ANDAVA ALLOCATO IL SECONDO NO PERCHE LA SOMMA DEI DUE ECCEDEVA LA MEMORIA TOTALE\n");
  }
  if(ptr8 == NULL && ptr9 != NULL){
    printf("ptr8: Fallita l'allocazione\n");
    printf("ptr9: allocazione riuscita\n");
    buddy_allocator_free_buddy(&buddy_allocator, ptr9);
    num_success++;
  }
  if(ptr8 != NULL && ptr9 == NULL){
    printf("ptr8: allocato correttemente\n");
    printf("ptr9: fallita allocazione poichè superata la memoria allocabile\n");
    buddy_allocator_free_buddy(&buddy_allocator, ptr8);
    num_success++;
  }
  
  
}

void test_buddy_allocator_free(){
  //test per vedere se la free funziona, alloco di blocchi di dimensione massima, se riesco ad allocarli entrambi in serie vuol dire che la free funziona

  Bit_map bitmap;
  uint8_t bitmap_buffer[BITSET_SIZE];
  //buffer di bitset va inizializzato come avente tutti i bit liberi
  for(int i = 0; i < BITSET_SIZE; i++){
    bitmap_buffer[i] = 1;
  }
  printf("\n\nbitmap_size: %d\n", BITSET_SIZE);
  bit_map_init(&bitmap, bitmap_buffer, BITSET_SIZE);
  
  // Creazione dell'allocator di buddy
  Buddy_allocator buddy_allocator;
  uint8_t mem_buffer [BUDDY_SIZE];
  int num_levels = NUM_LEVELS;
  int minimum_block_size = MINIMUM_BLOCK_SIZE;
  buddy_allocator_init(&buddy_allocator, mem_buffer, &bitmap, num_levels, minimum_block_size);
  printf("num_levels: %d\n", num_levels);
  printf("minimum_block_size: %d", minimum_block_size);
 
  void *ptr5 = buddy_allocator_alloc(&buddy_allocator, 1048560);
  if (ptr5 != NULL) {
    printf("ptr5: Allocazione di un blocco di dimensioni massime (layer 0) riuscita\n");
    buddy_allocator_free_buddy(&buddy_allocator, ptr5);
  } else {
    printf("ptr5: ERRORE Allocazione non riuscita\n");
  }


  void *ptr6 = buddy_allocator_alloc(&buddy_allocator, 1048560);
  if (ptr6 != NULL) {
    printf("ptr6: Allocazione di un blocco di dimensioni massime (layer 0) riuscita\n");
    buddy_allocator_free_buddy(&buddy_allocator, ptr6);
    num_success++;
  } else {
    printf("ptr6: ERRORE Allocazione non riuscita\n");
  }
}
int main() {

  test_buddy_allocator();
  test_buddy_allocator_free();
  printf("\nNUMERO DI TEST RIUSCITI: %d/%d\n", num_success, num_test);
  return 0;

}
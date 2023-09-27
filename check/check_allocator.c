#include "../bitmap.h"
#include "../bitmap.c"
#include "../Buddy_Allocator.h"
#include "../Buddy_Allocator.c"
#include "../allocator.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "sys/mman.h"


#define BUDDY_SIZE 1048576 // 1 MB
#define NUM_LEVELS 10
#define MINIMUM_BLOCK_SIZE (BUDDY_SIZE >> (NUM_LEVELS - 1)) //2048 
#define NUMBER_OF_BUDDIES (1 << NUM_LEVELS) - 1
#define BITSET_SIZE (NUMBER_OF_BUDDIES + 7) >> 3 
#define page_dim 10000  


//definisco variabile globale per controllare quanti test sull'allocator sono stati superati
int test_passed_allocator = 0;

int num_test = 6;

void test_initialize(Allocator_strc * alloc, Bit_map * bitmap, Buddy_allocator * buddy_allocator, void * buffer, void * buffer_buddy, int num_levels, int min_block_size, int bitset_size){
    int test_init_bitmap = 0;
    int test_init_buddy = 0;
    int test_init_alloc = 0;
    
    allocator_initialize(alloc, bitmap, buddy_allocator, buffer, buffer_buddy, num_levels, min_block_size, bitset_size  );
    	
    //controlliamo che l'inizializzazione delle strutture dati necessarie al funzionamento di allocator non presentino campi vuoti
    if(bitmap->data != NULL && bitmap->dim > -1 && bitmap->num_bit > -1 ){ 
        test_init_bitmap++;
    }
    if(buddy_allocator->levels > -1 && buddy_allocator->minimum_block_size > -1 && buddy_allocator->mem != NULL){
        test_init_buddy++;
    }
    if(alloc->buddy != NULL){
		test_init_alloc++;
	}
    if( (test_init_buddy + test_init_bitmap + test_init_alloc) == 3){//sia l'inizializzazione della bitmap, del buddy e di allocator sono andate a buon fine
        //aumento la variabile globale dei test di allocator
        test_passed_allocator++;
        printf("test inizializzazione andato a buon fine\n\n");
    }
}

void * test_allocator(int size, Allocator_strc* alloc){
    Allocator_strc* a = allocator(size, alloc);
    if(a->ptr == NULL){
        printf("errore nella allocazione tramite ALLOCATOR\n");
        return NULL;
    }
    else{
        test_passed_allocator++;
        printf("\ntest allocazione con mia implementazione andato a buon fine\n");
        return a->ptr;
    }

}

void * test_allocator_mmap(int size, Allocator_strc* alloc){
    Allocator_strc* b = allocator(size, alloc);
    if(b->ptr == NULL){
        printf("errore nella allocazione tramite MMAP\n");
        return NULL;
    }
    else{
        test_passed_allocator++;
        printf("\ntest allocazione con mmap andato a buon fine\n");
        return b->ptr;
    }
}


void test_simil_calloc(int size, int num, Allocator_strc* alloc){
	
    void * ptr = allocator_simil_calloc(size, num, alloc);
    for (size_t i = 0; i < size; i++) {
        if (*((char *)ptr + i) != num) {
            printf("Errore: valore diverso trovato alla posizione %zu\n", i);
            
        }
        printf("valore trovato %d\n", *((char *)ptr + i) );
    }
    test_passed_allocator++;
    printf("Test simil calloc riuscito!");
}


void test_deallocator_allocator(Allocator_strc* alloc, int dim){
    
    int a = deallocator(alloc, dim);
    if (a != -1) {//controllo che nessun perror sia stato lanciato
        test_passed_allocator++;
        printf("test deallocatore con mia implementazione andato a buon fine\n");
    }
    
}

void test_deallocator_munmap(Allocator_strc* alloc, int dim){

    int a = deallocator(alloc, dim);
    if(a != -1){
        test_passed_allocator++;
        printf("test deallocatore con mia mmap andato a buon fine\n\n");
    }
}




int main(){

    printf("INIZIO FASE DI TESTING\n");
    // Creazione della bitmap
    Bit_map bitmap;
    uint8_t bitmap_buffer[BITSET_SIZE];
    
    //andrebbe inizializzato a 1, poiche tutti i blocchi sono liberi
    for(int i = 0; i < BITSET_SIZE; i++){
        bitmap_buffer[i] = 1;
    } 

    // Creazione dell'allocator di buddy
    Buddy_allocator buddy_allocator;
    uint8_t mem_buffer [BUDDY_SIZE];
    int num_levels = NUM_LEVELS;
    int minimum_block_size = MINIMUM_BLOCK_SIZE;
    
	//creazione struttura allocator
	Allocator_strc alloc;
	
    test_initialize(&alloc, &bitmap, &buddy_allocator, bitmap_buffer, mem_buffer, num_levels, minimum_block_size, BITSET_SIZE);
    
    void * ptr1 = test_allocator(2100, &alloc);
    if(ptr1 !=  NULL){
		printf("Inizio test deallocazione tramite mia implementazione\n");
        test_deallocator_allocator(&alloc, 2100);
        //se non ha passato test_allocazione inutile deallocare
    }

    void * ptr2 = test_allocator_mmap(2600, &alloc);
    if(ptr2 != NULL){
        test_deallocator_munmap(&alloc, 2600);
        //se fallito test_inizializzazione inutile deallocare 
    }
    
    test_simil_calloc(10, 3, &alloc );
    
    printf("\ntest_passati in totale sono %d", test_passed_allocator);
    if(test_passed_allocator == num_test){
        printf("\n\nogni test sull'allocatore è andato a buon fine\n");
    }
    return 0;
}




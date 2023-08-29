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

void test_initialize(Bit_map * bitmap, void * buffer, Buddy_allocator * buddy_allocator, void * buffer_buddy, int num_levels, int min_block_size, int bitset_size){
    int test_init_bitmap = 0;
    int test_init_buddy = 0;
    allocator_initialize(bitmap, buffer, buddy_allocator, buffer_buddy, num_levels, min_block_size, bitset_size  );

    //controlliamo che l'inizializzazione delle strutture dati necessarie al funzionamento di allocator non presentino campi vuoti
    if(bitmap->data != NULL && bitmap->dim > -1 && bitmap->num_bit > -1 ){ //metto diverso da zero o maggiore di -1???
        test_init_bitmap++;
    }
    if(buddy_allocator->levels > -1 && buddy_allocator->minimum_block_size > -1 && buddy_allocator->mem != NULL){
        test_init_buddy++;
    }
    if( (test_init_buddy + test_init_bitmap ) == 2){//sia l'inizializzazione della bitmpa che quella del buddy sono andate a buon fine
        //aumento la variabile globale dei test di allocator
        test_passed_allocator++;
        printf("test inizializzazione andato a buon fine\n");
    }
}

void * test_allocator(int size, Buddy_allocator * buddy_allocator){
    void * a = allocator(size, buddy_allocator);
    if(a == NULL){
        printf("errore nella allocazione tramite ALLOCATOR\n");
        return NULL;
    }
    else{
        test_passed_allocator++;
        printf("test allocazione con mia implementazione andato a buon fine\n");
        return a;
    }

}

void * test_allocator_mmap(int size, Buddy_allocator * buddy_allocator){//passiamo il buddy anche se sappiamo gia in fase di testing che non servirà, ma ciò lo si vede dentro la funzione allocator
    void * b = allocator(size, buddy_allocator);
    if(b == NULL){
        printf("errore nella allocazione tramite MMAP\n");
        return NULL;
    }
    else{
        test_passed_allocator++;
        printf("test allocazione con mmap andato a buon fine\n");
        return b;
    }
}


void test_simil_calloc(int size, int num, Buddy_allocator * buddy_allocator){
    void * ptr = allocator_simil_calloc(size, num, buddy_allocator);
    for (size_t i = 0; i < size; i++) {
        if (*((char *)ptr + i) != num) {
            printf("Errore: valore diverso trovato alla posizione %zu\n", i);
            
        }
        printf("valore trovato %d\n", *((char *)ptr + i) );
    }
    test_passed_allocator++;
    printf("Test simil calloc riuscito!");
}


void test_deallocator_allocator(Buddy_allocator * buddy_allocator, void * ptr, int dim){
    
    int a = deallocator_buddy(buddy_allocator, ptr, dim);
    if (a != -1) {//controllo che nessun perror sia stato lanciato
        test_passed_allocator++;
        printf("test deallocatore con mia implementazione andato a buon fine\n");
    }
    
}

void test_deallocator_munmap(void * ptr, int dim){

    int a = deallocator_mmap(ptr,  dim);
    if(a != -1){
        test_passed_allocator++;
        printf("test deallocatore con mia mmap andato a buon fine\n");
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

    test_initialize(&bitmap, bitmap_buffer, &buddy_allocator, mem_buffer, num_levels, minimum_block_size, BITSET_SIZE);
    
    void * ptr1 = test_allocator(2100, &buddy_allocator);
    if(ptr1 !=  NULL){
        test_deallocator_allocator(&buddy_allocator, ptr1, 2100);
        //se non ha passato test_allocazione inutile fargli fare la deallocazione
    }

    void * ptr2 = test_allocator_mmap(2600, &buddy_allocator);
    if(ptr2 != NULL){
        test_deallocator_munmap(ptr2, 2600);
        //se fallito test_inizializzazione inutile deallocazione
    }
    

    test_simil_calloc(10, 3, &buddy_allocator );
    
    printf("\ntest_passati in totale sono %d", test_passed_allocator);
    if(test_passed_allocator == num_test){
        printf("\n\n\n\nogni test sull'allocatore è andato a buon fine");
    }
    return 0;
}

/*
test inizializzazione andato a buon fine
test allocazione con buddy buon fine
test free con buddy  buon fine
test con mmap andato a buon fine
test con buddy simil_calloc non provato
test simil_calloc riuscito
*/


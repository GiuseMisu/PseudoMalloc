//Importiamo le librerie necessarie 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h> 
#include <errno.h>
#include <string.h>

//librerie necessarie per allocator
#include "bitmap.h"
#include "Buddy_Allocator.h"
#include "allocator.h"

#define page_dim 10000  //valore arbitrario 


//funzione che inizializza allocator se date bit_map, buffer e buddy_allocator
void allocator_initialize(Allocator_strc * alloc,Bit_map * bitmap,  Buddy_allocator * buddy_allocator ,void * buffer, void * buffer_buddy, int num_levels, int min_block_size, int bitset_size){
	alloc -> buddy = buddy_allocator;
	
    bit_map_init(bitmap, buffer, bitset_size);
    buddy_allocator_init(buddy_allocator, buffer_buddy, bitmap, num_levels, min_block_size);
}


//funzione che alloca la memoria
Allocator_strc *allocator(int size, Allocator_strc * alloc){
    if(size < 1){
        return NULL;
    }
    else{
        size = size + sizeof(size);//bisogna aggiungere alla size anche la dimensione 
        //in base alla dimensione richiesta decidiamo se effetturare allocazione tramite mmap o allocator
        if(size > page_dim/4){
            printf("\nla dimensione è %d, quindi è più grande della dimensione di un quarto di pagina %d \n", size, page_dim/4);
            void * pointer = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

            if(pointer == MAP_FAILED){
                errno = ENOMEM;//not enough memory
                perror("error allocator: mmap failed");
                return NULL;
            }
            else{
				alloc->ptr = pointer;//mi salvo nella struttura Allocator_strc il puntatore alla zona appena allocata
                return alloc;
            }
        }
        else{
			Buddy_allocator * buddy = alloc->buddy;
            void *a = buddy_allocator_alloc(buddy, size); 
            if(a == NULL){
                perror("error allocator: buddy_allocator_alloc failed");
                return NULL;
            }
            else{
				alloc->ptr = a;//mi salvo nella struttura Allocator_strc il puntatore alla zona appena allocata
                return alloc;
            } 
        }
    }
}


//nuova versione della funzione deallocator, viene chiamata per deallocare a prescindere dalla tipologia di allocazione 
int deallocator(Allocator_strc * alloc, int dim){
	
	if(dim < 1|| alloc == NULL){
        return -1;
    }
   
    void * puntatore = alloc->ptr;
    Buddy_allocator* b = alloc->buddy;
    
    if(puntatore == NULL || b == NULL){
		return -1;
	}
    
    uint8_t *first_index_buddy = &b->mem[0];//voglio sapere indirizzo di memoria del primo elemento del buffer di memoria del buddy
    uint8_t *last_index_buddy = first_index_buddy + ( 1 << (b->levels) ) * b->minimum_block_size - 1; //mi calcolo ultimo indirizzo di memoria del buddy conoscendo il numero di livelli e la dimensione minima del blocco
	
	//controlla il puntatore restituisto da allocazione, per capire se questo punta a indirizzo che è dentro spazio degli indirizzi del buddy o se fuori buddy (per comprendere se dentro buddy prendo indirizzo del primo elemento della memoria del buddy e dell'ultimo)
	//se dentro allora dealloca con buddy, se fuori con munmap
	    
	if(	(uint8_t *) puntatore >= first_index_buddy && (uint8_t *)puntatore<= last_index_buddy){//caso in cui deallochi con buddy
		printf("Il puntatore pointer si trova all'interno del range del buddy.\n");
		buddy_allocator_free_buddy(b,puntatore); 
	}
	else{//caso munmap
		printf("Il puntatore pointer NON si trova all'interno del range del buddy.\n");
		dim = dim + sizeof(dim);
		if(munmap(puntatore, dim) == -1){ 
			perror("munmap");
			return -1;
		}
	}	
}




//Funzione che alloca la memoria e la inizializza con il valore num
void * allocator_simil_calloc(int size, int num, Allocator_strc * alloc){
    if(size < 1){
        return NULL;
    }
    else{
		Buddy_allocator * buddy = alloc->buddy;
        void *pointer = allocator(size, alloc)->ptr;
        if(pointer == NULL){
            perror("error allocator_simil_calloc: allocator failed");
            return NULL;
        }
        else{
            memset(pointer, num, size);
            return pointer;
        }
    }
}

//Importiamo le librerie necessarie 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h> 
#include <errno.h>
#include <string.h>

//Importiamo le librerie necessarie per allocator
#include "bitmap.h"
#include "Buddy_Allocator.h"
#include "allocator.h"

#define page_dim 10000  //valore arbitrario 


//funzione che inizializza allocator se date bit_map, buffer e buddy_allocator
void allocator_initialize(Bit_map * bitmap, void * buffer, Buddy_allocator * buddy_allocator, void * buffer_buddy, int num_levels, int min_block_size, int bitset_size){
    bit_map_init(bitmap, buffer, bitset_size);
    buddy_allocator_init(buddy_allocator, buffer_buddy, bitmap, num_levels, min_block_size);
}


//funzione che alloca la memoria
void *allocator(int size, Buddy_allocator * buddy_allocator){
    if(size < 1){
        return NULL;
    }
    else{
        size = size + sizeof(size);//bisogna aggiungere alla size anche la dimensione 
        //in base alla dimensione richiesta decidiamo se effetturare allocazione tramite mmap o allocator
        if(size > page_dim/4){
            printf("la dimensione è %d, quindi è più grande della dimensione di un quarto di pagina %d \n", size, page_dim/4);
            void * pointer = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);//mi segnala errore su map anonymous ma comunque funziona

            if(pointer == MAP_FAILED){
                errno = ENOMEM;//not enough memory
                perror("error allocator: mmap failed");
                return NULL;
            }
            else{
                return pointer;
            }
        }
        else{
            void *a = buddy_allocator_alloc(buddy_allocator, size); 
            if(a == NULL){
                perror("error allocator: buddy_allocator_alloc failed");
                return NULL;
            }
            else{
                return a;
            } 
        }
    }
}

//funzione che dealloca la memoria se allocazione fatta con mmap
//se allocazione stata fatta tramite mmap serve ptr e dim 
int deallocator_mmap(void *ptr, int dim){
    if(dim < 1 && ptr != NULL){
        return -1;
    }

    else{
        if(dim > page_dim/4){//let's use the munmap
            dim = dim + sizeof(dim);
            if(munmap(ptr, dim) == -1){
                perror("munmap");
                return -1;
            }
            

        }
        //else stai sbagliando a chiamare la funzione devi deallocare con buddy
    }
}


//funzione che dealloca la memoria se allocazione fatta tramite allocator
//se allocazione fatta con mia implementazione serve passare il buddy e il puntatore restituito da buddy_allocator_alloc()
int deallocator_buddy(Buddy_allocator * buddy_allocator, void *ptr, int dim){
    if(dim < 0){
        return -1;
    }
    else{
        if(dim <= page_dim/4){//let's use the buddy
            buddy_allocator_free_buddy(buddy_allocator,ptr);
            
        }
        else{//else stai sbagliando a deallocare devi usare munmap
            return -1;
        }
       
    }
}


//Funzione che alloca la memoria e la inizializza con il valore num
void * allocator_simil_calloc(int size, int num, Buddy_allocator * buddy_allocator){
    if(size < 1){
        return NULL;
    }
    else{
        void *pointer = allocator(size, buddy_allocator);
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

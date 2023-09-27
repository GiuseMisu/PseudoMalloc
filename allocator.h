#pragma once


typedef struct {
    Buddy_allocator * buddy;
    void * ptr; //puntatore alla zona di memoria allocata, restituito da funzione allocator a prescindere dalla tipologia di allocazione(se mmap o con buddy)
}Allocator_strc;

//Funzione che inizializza allocator
void allocator_initialize(Allocator_strc * alloc, Bit_map * bitmap, Buddy_allocator * buddy_allocator, void * buffer, void * buffer_buddy, int num_levels, int min_block_size, int bitset_size);

//Funzione che alloca la memoria, tipologia di allocazione dipende dalla dimensione richiesta
//Se dim minore di 1/4 della dimensione di una pagina allora si usa la mia implementazione, altrimenti si usa mmap
Allocator_strc *allocator(int size,  Allocator_strc * alloc);

//funzione che dealloca la memoria precedentemente allocata a prescindere dalla tipologia di allocazione effettuata
int deallocator(Allocator_strc * alloc, int dim);
	
//Funzione che alloca la memoria e la riempie con il valore passato da utente
void * allocator_simil_calloc(int size, int num, Allocator_strc * alloc);


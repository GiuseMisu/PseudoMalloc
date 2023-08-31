#pragma once

//Funzione che inizializza allocator
void allocator_initialize(Bit_map * bitmap, void * buffer, Buddy_allocator * buddy_allocator,  void * buffer_buddy, int num_levels, int min_block_size, int bitset_size);

//Funzione che alloca la memoria, tipologia di allocazione dipende dalla dimensione richiesta
//Se dim minore di 1/4 della dimensione di una pagina allora si usa la mia implementazione, altrimenti si usa mmap
void *allocator(int size, Buddy_allocator * buddy_allocator);

//funzione che dealloca la memoria se stata allocata con mmap (dim > 1/4 page_size)
int deallocator_mmap(void *ptr, int dim);

//Funzione che dealloca memoria se stata allocata tramite mia impl
//se allocazione fatta con mia implementazione serve passare il buddy e il puntatore restituito da buddy_allocator_alloc()
int deallocator_buddy(Buddy_allocator * buddy_allocator, void *ptr, int dim);

//Funzione che alloca la memoria e la riempie con il valore passato da utente
void * allocator_simil_calloc(int size, int num, Buddy_allocator * buddy_allocator);
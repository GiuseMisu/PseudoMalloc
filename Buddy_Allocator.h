#pragma once

// buddy allocator, implemented with a bitmap that manages 1 MB of memory for these small allocations.

#include "bitmap.h"

typedef struct {
    Bit_map *bitmap; // puntatore alla bitmap (guarda bitmap.h)
    void *mem; // puntatore alla memoria allocata
    int levels; // livelli dell'albero
    int size; // dimensione della memoria allocata //forse da rimuovere tanto lo ottieni da mem 
    int minimum_block_size; // dimensione minima che può assumere blocco
    int *free_blocks; // array di interi che tiene traccia dei blocchi liberi
}Buddy_allocator;

int get_parent_index(int index);

int from_index_to_level(int index);

int get_left_child_index(int index);

int get_right_child_index(int index);

int offset_from_first(int index);

int first_index_from_level(int level);

// inizializza la struttura dati buddy allocator
void buddy_allocator_init(Buddy_allocator * buddy_allocator, void * buffer, Bit_map * bitmap, int levels, int size);

//dato un livello restituisce la prima posizione libera
int search_free_block(Bit_map * bitmap, int level);

//trova il livello contenente il blocco di dimensione minima in grado di contenere elemento di dimensioni dim
int search_free_level(Buddy_allocator * buddy_allocator, int dim);

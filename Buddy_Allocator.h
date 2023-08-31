#pragma once
#include "bitmap.h"

typedef struct {
    Bit_map *bitmap; // puntatore alla bitmap (guarda bitmap.h)
    void *mem; // puntatore alla memoria allocata
    int levels; // livelli dell'albero
    int minimum_block_size; // dimensione minima che può assumere blocco
}Buddy_allocator;

int get_parent_index(int index);

int from_index_to_level(int index);

int get_left_child_index(int index);

int get_right_child_index(int index);

int offset_from_first(int index);

int first_index_from_level(int level);

int get_buddy_index(int index);

//controlla che siano rispettate le precondizioni per l'indice
void check_index(Buddy_allocator * buddy_allocator, int index);

// inizializza la struttura dati buddy allocator
void buddy_allocator_init(Buddy_allocator * buddy_allocator, void * buffer, Bit_map * bitmap, int levels, int size);

//dato un livello restituisce la prima posizione libera su quel livello, se non ci sono posizioni libere restituisce -1
int search_free_block(Bit_map * bitmap, int level);

//trova il livello contenente il blocco di dimensione minima in grado di contenere elemento di dimensioni dim
int search_free_level(Buddy_allocator * buddy_allocator, int dim);

// cerca di trovare un blocco libero all'interno del buddy_allocator, andando a scandire tutti i livelli
// se lo trova restituisce l'indice del blocco altrimenti -1
int buddy_allocator_finder(Buddy_allocator * buddy_allocator, int level);

// libera un determinato buddy in posizione index e controlla pure se liberando lui, si puo liberare il padre ecc..
void buddy_allocator_unleash(Buddy_allocator * buddy_allocator, int index);

// alloca un blocco di dimensione dim, restituisce l'indirizzo del primo byte dopo l'header, quindi inizio del blocco di memoria allocato
void * buddy_allocator_alloc(Buddy_allocator * buddy_allocator, int dim);

// libera il blocco di memoria allocato in precedenza
void buddy_allocator_free_buddy(Buddy_allocator * buddy_allocator, void * ptr);
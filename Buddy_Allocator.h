#pragma once

// buddy allocator, implemented with a bitmap that manages 1 MB of memory for these small allocations.

#include "bitmap.h"

typedef struct {

    int livelli; // livelli dell'albero
    int size; // dimensione della memoria allocata
    int min_size; // dimensione minima di un blocco
    void *mem; // puntatore alla memoria allocata
    Bit_map *bitmap; // puntatore alla bitmap (guarda bitmap.h)
    int *free_blocks; // array di interi che tiene traccia dei blocchi liberi



}BuddyAllocator;
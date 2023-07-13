#pragma once
#include <stdint.h>

typedef struct {
    int dim;    
    int num_bit;
    uint8_t * data;
} Bit_map;

/*inizializza la struttura dati bitmap con un buffer di dimensione dim_buffer
param:
    bm: puntatore alla struttura dati da inizializzare
    buff: puntatore al buffer da utilizzare
    dim_buffer: dimensione del buffer
*/
void bit_map_init(Bit_map * bm, uint8_t * buff ,int dim_buffer);

/*
setta il bit in posizione index in base al valore di boolean
param:
    bm: puntatore alla struttura dati da utilizzare
    index: posizione del bit da settare
    boolean: valore del bit da settare, 0 o 1
*/
void bit_map_set(Bit_map * bit_map, int index, int boolean);

/*
ritorna il valore del bit in posizione index
param:
    bm: puntatore alla struttura dati da utilizzare
    index: posizione del bit da ritornare
*/
int bit_map_get(Bit_map * bit_map, int index);




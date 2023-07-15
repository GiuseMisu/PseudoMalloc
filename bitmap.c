#include "bitmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>


void bit_map_init(Bit_map * bm, uint8_t * buff ,int dim_buffer){
    bm->data = buff; // puntatore al buffer
    bm->dim = dim_buffer; // dimensione del buffer
    bm->num_bit = dim_buffer * 8; // numero di bit
}

void bit_map_set(Bit_map * bm, int pos, int boolean){

    if(pos >= bm->num_bit){ // se la posizione è maggiore del numero di bit
        errno = EINVAL; // setto errno come invalid argument
        perror("error bit_map_set"); // stampo l'errore
        //exit(EXIT_FAILURE); // esco
    }

    int byte = pos / 8; // calcolo il byte, cioè specifica l'indice del byte che contiene il bit da impostare
    int bit = pos % 8; // calcolo il bit, cioè specifico offset all'interno del byte

    if(boolean == 0){ // se boolean è 0
        bm->data[byte] &= ~(1 << bit); // resetto il bit a 0 
    }
    else{
        bm->data[byte] |= 1 << bit; // setto il bit a 1 
    }
}

int bit_map_get(Bit_map * bm, int pos){

    if(pos >= bm->dim*8){ 
        printf("numero di bit superato massimo  %d\n", bm->dim*8);
        perror("error bit_map_get"); // stampo l'errore
        //exit(EXIT_FAILURE); // esco
    }

    int byte = pos / 8; // calcolo il byte, indice del byte che contiene il bit da prendere
    int bit = pos % 8; // calcolo il bit, cioè specifico offset all'interno del byte

    return (bm->data[byte] >> bit) & 1; // ritorno il bit in posizione pos*/
    
}

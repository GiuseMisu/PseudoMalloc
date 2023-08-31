/*L1. Pseudo Malloc:
   This is a malloc replacement
   The system relies on mmap for the physical allocation of memory, but handles the requests in
   2 ways:
   - for small requests (< 1/4 of the page size) it uses a buddy allocator.
     Clearly, such a buddy allocator can manage at most page-size bytes
     For simplicity use a single buddy allocator, implemented with a bitmap
     that manages 1 MB of memory for these small allocations.

   - for large request (>=1/4 of the page size) uses a mmap.*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "bitmap.h"
#include "Buddy_Allocator.h"

int get_parent_index(int index){
  return (int)((index - 1) / 2);
}

int from_index_to_level(int index){
  return (int)log2(index);
}

int get_left_child_index(int index){
  return (int)((index) * 2 + 1);  
}

int get_right_child_index(int index){
  return (int)((index) * 2 + 2);  
}

int offset_from_first(int index){
  return (int)(index - ( (1 << from_index_to_level(index)) - 1) ); 
}

int first_index_from_level(int level){
  return (int)(1 << level) - 1; 
}

int get_buddy_index(int index){
  if(index % 2 == 0){
    return index - 1;
  }
  else{
    return index + 1;
  }
}

void check_index(Buddy_allocator * buddy_allocator, int index){
  if(index < 0 || index > (1 << buddy_allocator->levels) - 1){
    perror("error buddy_allocator_unleash: index not valid");
    //exit(EXIT_FAILURE);
  }
}

void buddy_allocator_init(Buddy_allocator * buddy_allocator, void * buffer, Bit_map * bitmap, int levels, int min){
  buddy_allocator->mem = buffer;
  buddy_allocator->bitmap = bitmap;
  buddy_allocator->levels = levels;
  buddy_allocator->minimum_block_size = min;
}

int search_free_block(Bit_map * bitmap, int level){
  printf("livello passato al free block: %d\n", level);
  int starting_index = first_index_from_level(level);
  int ending_index = first_index_from_level(level + 1) ;//-1 perche primo livello deve esserci un solo indice e non 2
  printf("\tstarting_index: %d\n", starting_index);
  printf("\tending_index: %d\n", ending_index);
  //while loop fino a che non trovo un blocco libero
  int i = starting_index;
  while(i <= ending_index){
    printf("i: %d\n", i);
    printf("bit_map_get(bitmap, i): %d\n", bit_map_get(bitmap, i));
    //vai a controllare lo stato di allocazione di un blocco nella bitmap
    if(bit_map_get(bitmap, i) != 0){
      printf("trovato un blocco libero in pos: %d\n", i);
      return i;
    }
    i++;
  }
  return -1;//se non trovo un blocco libero ritorno -1
}

int search_free_level(Buddy_allocator * buddy_allocator, int dim){
  printf("dim search_free_level: %d\n", dim);
  int lvl = buddy_allocator->levels - 1; //-1 perchè l'indice parte da 0
  int starting_dim = buddy_allocator->minimum_block_size;
  while(lvl >= 0){
    printf("\tdimensioni attuali al livello %d, sono: %d\n", lvl, starting_dim);
    if(dim <= starting_dim){
      return lvl;
    }
    lvl--;
    starting_dim = starting_dim * 2;
  }
  return lvl;
}


//funzione implementata ricorsivamente
int buddy_allocator_finder(Buddy_allocator * buddy_allocator, int lvl){
  printf("buddy allocator finder, %d\n", lvl);
  if(lvl < 0){
    return -1;
  }
  int buddy_index = search_free_block(buddy_allocator->bitmap, lvl);
   
  if(buddy_index != -1){//hai trovato un blocco libero al livello corrente, setti nella bit map come occupato e restituisci index in maniera tale che la alloc possa allocare in quella posizione
    bit_map_set(buddy_allocator->bitmap, buddy_index, 0);
    return buddy_index;
  }

  printf("non hai trovato un buddy libero al livello %d\n", lvl);
  int parent_index = buddy_allocator_finder(buddy_allocator, lvl - 1);
  
  if(parent_index == -1){
    return -1;
  }
  printf("hai trovato un blocco libero al livello precedente: %d\n", lvl - 1);  
  int right_index = get_right_child_index(parent_index);
  check_index(buddy_allocator, right_index);  

  int left_index = get_left_child_index(parent_index);
  check_index(buddy_allocator, left_index);

  bit_map_set(buddy_allocator->bitmap, right_index, 1);
  return left_index;
}


void buddy_allocator_unleash(Buddy_allocator * buddy_allocator, int index){
  //controlla la correttezza dell'indice passato
  check_index(buddy_allocator, index);

  //setta il blocco come libero
  bit_map_set(buddy_allocator->bitmap, index, 1);

  //fai ciclo per vedere se nel caso in cui tuo fratello fosse libero puoi fare merge e andare a liberare anche il padre
  while (index != 0){
      int parent_idx = get_parent_index(index);
      check_index(buddy_allocator, parent_idx);

      int left_index = get_left_child_index(parent_idx);
      check_index(buddy_allocator, left_index);

      int right_index = get_right_child_index(parent_idx);
      check_index(buddy_allocator, right_index);

      //controllo se possibile fare operazione di merge
      if (bit_map_get(buddy_allocator->bitmap, left_index ) == 1 && bit_map_get(buddy_allocator->bitmap, right_index) == 1){
          //se i due figli sono liberi allora libero anche il padre
          bit_map_set(buddy_allocator->bitmap, parent_idx, 1);
          //setto come occupati i due buddies fratelli
          bit_map_set(buddy_allocator->bitmap, left_index, 0);
          bit_map_set(buddy_allocator->bitmap, right_index, 0);
      }
      else{
          break;
      }
      index = parent_idx;
  }
}

void * buddy_allocator_alloc(Buddy_allocator * buddy_allocator, int dim){
  
  int level = search_free_level(buddy_allocator, dim + sizeof(int));//dimensioni ci va sommato l'header del blocco di memoria che è di 4 byte (int)
  if(level == -1){
    printf("attraverso la funzione search free level non si e' trovato uno spazio abbastanza grande per allocare il blocco richiesto\n");
    perror("error buddy_allocator_alloc: no memory available");
    return NULL;
  }
  printf("sto passando al finder livello: %d, cosi che cerca in quel livello uno spazio vuoto\n", level);
  int index = buddy_allocator_finder(buddy_allocator, level);
  if(index == -1){//finder non ha trovato un blocco libero
    perror("error buddy_allocator_alloc: no memory available");
    return NULL;
  }
  //vuol dire che find ha trovato un blocco libero e ora devo allocarlo
  else{
    int offset = offset_from_first(index);
    int starting_dim = buddy_allocator->minimum_block_size; //dimensione del blocco di memoria più piccolo
    int i = 0;
    while(i < level){//itero finchè non arrivo al livello del blocco di memoria richiesto
      offset = offset * 2;
      //ad ogni iterazione la dimensione del blocco di memoria raddoppia
      starting_dim = starting_dim * 2;
      i++;
    }
    int * header = (int *) (buddy_allocator->mem + offset);

    *header = index; //salvo l'indice del blocco di memoria allocato, cosi con la free posso trovare il blocco di memoria da liberare
    return (void *) (header + 1); 
  }
}

void buddy_allocator_free_buddy(Buddy_allocator * buddy_allocator, void * ptr){
  int * header = (int *) ptr - 1;
  int index = *header;
  
  //controlli sulla correttezza dell'indice
  check_index(buddy_allocator, index);
  buddy_allocator_unleash(buddy_allocator, index);
}




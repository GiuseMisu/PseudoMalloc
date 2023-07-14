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

#include "Buddy_Allocator.h"

int get_parent_index(int index){
  return (int)((index - 1) / 2);
}

int from_index_to_level(int index){
  return (int)log2(index);
}

int get_left_child_index(int index){
  return (int)((index) * 2 +1); 
}

int get_right_child_index(int index){
  return (int)((index) * 2 + 2); 
}

int offset_from_first(int index){
  return (int)(index - (1 << index));
}

int first_index_from_level(int level){
  return (int)(1 << level);
}

int get_buddy_index(int index){
  if(index % 2 == 0){
    return index - 1;
  }
  else{
    return index + 1;
  }
}

void buddy_allocator_init(Buddy_allocator * buddy_allocator, void * buffer, Bit_map * bitmap, int levels, int size){
  buddy_allocator->mem = buffer;
  buddy_allocator->bitmap = bitmap;
  buddy_allocator->levels = levels;
  buddy_allocator->size = size;
}

int search_free_block(Bit_map * bitmap, int level){
  int starting_index = first_index_from_level(level) - 1;
  int ending_index = first_index_from_level(level + 1) - 1;

  //while loop fino a che non trovo un blocco libero
  int i = starting_index;
  while(i < ending_index){
    if(bit_map_get(bitmap, i) == 1){
      return i;
    }
    i++;
  }
  return -1;//se non trovo un blocco libero ritorno -1
}

int search_free_level(Buddy_allocator * buddy_allocator, int dim){
  int lvl = buddy_allocator->levels - 1; //-1 perchè l'indice parte da 0
  int starting_dim = buddy_allocator->minimum_block_size;
  while(lvl >= 0){
    if(dim <= starting_dim){
      return lvl;
    }
    lvl--;
    starting_dim = starting_dim * 2;
  }
  return lvl;
}

int buddy_allocator_finder(Buddy_allocator * buddy_allocator, int lvl){
  if(lvl < 0){
    return -1;
  }
  while(lvl >= 0){
    int buddy_index = search_free_block(&buddy_allocator->bitmap, lvl);
//hai trovato un blocco libero al livello corrente
    if(buddy_index != -1){
      bit_map_set(&buddy_allocator->bitmap, buddy_index, 0);
      return buddy_index;
    }
//non hai trovato un blocco libero al livello corrente
    if (lvl > 0){ 
			int parent_index = search_free_block(&buddy_allocator->bitmap, lvl - 1);
      if (parent_index != -1) {
        //indichiamo che il blocco padre è occupato
        bit_map_set(&buddy_allocator->bitmap, parent_index, 0);
        int left_child_index = get_left_child_index(parent_index);
        int right_child_index = get_right_child_index(parent_index);
        //indichi che i blocchi figli sono liberi
        bit_map_set(&buddy_allocator->bitmap, left_child_index, 1);
        bit_map_set(&buddy_allocator->bitmap, right_child_index, 1);
        return left_child_index;
      }
      else {//se non hai trovato un blocco libero al livello corrente e al livello precedente vuol dire che non hai memoria disponibile
        return -1;
      }
    }
    lvl--;
  }
}

void buddy_allocator_unleash(Buddy_allocator * buddy_allocator, int index){

  //controlla la correttezza dell'indice passato
  if(index < 0 || index > (1 << buddy_allocator->levels) - 1){
    perror("index not valid");
    //exit(EXIT_FAILURE);
  }

	while (index != 0){
    int parent_index = get_parent_index(index);
		int brother_index = get_buddy_index(index);

		// se fratello del blocco in posizione index è libero allora puoi fare merge (segni padre come libero e fratello come occupato)
		if (bitmap_get_bit(&buddy_allocator->bitmap, brother_index)){
			// libero il padre
			bitmap_set_bit(&buddy_allocator->bitmap, parent_index, 1);
      // setto come occupati i due buddies fratelli
			bitmap_set_bit(&buddy_allocator->bitmap, index, 0);
			bitmap_set_bit(&buddy_allocator->bitmap, brother_index, 0);
			// vanno aggiornati gli indici cosi alla prossima iterazione si controlla il padre del padre
			index = parent_index;
			parent_index = (index - 1) / 2;
		}
		else{ //se fratello non libero allora non puoi fare merge, liberi solo buddy in posizione index
			bitmap_set_bit(&buddy_allocator->bitmap, index, 1);
			break;
		}
	}

}




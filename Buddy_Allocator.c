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
  return (int)((index) / 2);
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
  return -1;
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





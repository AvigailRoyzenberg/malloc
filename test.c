
#include <stdio.h>
#include "my_malloc.h"
#include <stdlib.h>

int main(int argc, char **argv){
  unsigned int global_mem_size = 1024 * 1024;
  unsigned char *global_memory = malloc(global_mem_size);

  mem_init(global_memory, global_mem_size);
  mem_stats();

  const size_t PTRS = 10;
  unsigned char *ptr_array[PTRS];
  unsigned int sizes[] = {50, 20, 20, 20, 50, 0};

  for (int i = 0; sizes[i] != 0; i++) {
    char buf[1024];
    ptr_array[i] = my_malloc(sizes[i]);
    mem_stats(buf);
  }
  test_mem_consistency();
  printf("\nMemory consistency tested");

  my_free(ptr_array[1]); 
  printf("\nafter free #1\n"); 
  mem_stats();

  my_free(ptr_array[3]);  
  printf("\nafter free #3\n");
  mem_stats();

  my_free(ptr_array[2]); 
  printf("\nafter free #2\n"); 
  mem_stats();

  my_free(ptr_array[0]);  
  printf("\nafter free #0\n");
  mem_stats();

  //my_free(ptr_array[4]); 
  // printf("\nafter free #4\n");
  //mem_stats();

  
  return 0;
}

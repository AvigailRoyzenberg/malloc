#include <stdio.h>
#include "my_malloc.h"
#include <stdlib.h>

// const size_t PTRS = 10;

// int main(int argc, char ** argv) {
//   printf("Hello World");
//   void * ptrs[PTRS];
//   memconsistency();
//   for (size_t i = 0; i < PTRS; ++i) {
//     memstats();
//     ptrs[i] = my_malloc(1 << i);
//     memstats();
//   }
//   // memconsistency();
//   // printf("\ndaniel says hi");
//   // for (size_t i = 0; i < PTRS; i += 2) {
//   //   my_free(ptrs[i]);
//   // }
//   // memconsistency();
//   // printf("\nsecond loop sponsered by daniel");
//   // for (size_t i = 1; i < PTRS; i += 2) {
//   //   my_free(ptrs[i]);
//   // }
//   // memconsistency();
//   // printf("\nThird loop for daniel");
//   // for (size_t i = 0; i < PTRS; ++i) {
//   //   ptrs[i] = my_malloc(1 << (PTRS-i-1));
//   // }
//   // memconsistency();
//   // printf("\nthis works!\n");
//   // my_malloc(10);
//   // memstats();
//   return 0;
  
// }


int main(int argc, char **argv){
  unsigned int global_mem_size = 1024 * 1024;
  unsigned char *global_memory = malloc(global_mem_size);

  mem_init(global_memory, global_mem_size);
  memstats();

  unsigned char *ptr_array[10];
  unsigned int sizes[] = {50, 20, 20, 20, 50, 0};

  for (int i = 0; sizes[i] != 0; i++) {
    char buf[1024];
    ptr_array[i] = my_malloc(sizes[i]);
    
    sprintf(buf, "after iteration %d size %d", i, sizes[i]);
    memstats(buf);
  }

  my_free(ptr_array[1]); 
  printf("after free #1"); 
  memstats();

  my_free(ptr_array[3]);  
  printf("after free #3");
  memstats();

  my_free(ptr_array[2]); 
  printf("after free #2"); 
  memstats();


  my_free(ptr_array[0]);  
  printf("after free #0");
  memstats();

  my_free(ptr_array[4]); 
  printf("after free #4");
  memstats();
}

// my version:

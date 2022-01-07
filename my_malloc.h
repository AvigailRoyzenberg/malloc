#include <unistd.h>
void * my_malloc(size_t n);
void my_free(void * ptr);
void mem_stats();
void test_mem_consistency();

#include <unistd.h>
#ifndef NDEBUG
#include <stdio.h>
#endif
#include "my_malloc.h"

// Defining a memory block with members: occupied, size of the memory block, and data
struct mem_block {  
  char occupied;
  size_t length;    
  char data[0];
};

// Defining a memory block with members: occupied, size of free mem block, a pointer to the next free mem block, a pointer to the previous one
struct free_mem_block {                                            
  char occupied;                                                   
  size_t length;
  struct free_mem_block * next_free;
  struct free_mem_block * prev_free;
};

static struct mem_block * first = NULL;                            // decalring a memory block structure that's a pointer named first
static struct mem_block * last = NULL;                             // declaring a memory block structure that's a pointer named last
static struct free_mem_block * last_free = NULL;                   // declaring a memory block structure that's a pointer named last_free
static const size_t data_offset = sizeof(struct mem_block);        // declaring data_offset as the length of memory block
static const size_t min_size = 2*sizeof(struct free_mem_block *);  // declaring min_size as 2x the length of a free memory block


static struct mem_block * new_block(size_t sz) {                   // Defining a mem_block structure type pointer called new_block
  struct mem_block * res = (struct mem_block *) sbrk(sz);          // Using sbrk to manipulate the heap and ask for space
  res->occupied = 1;
  res->length = sz;
  return res;
}

#define next_block(cur) ((struct mem_block *) (((char *) cur) + cur->length))


// This routine is guaranteed to be called before any of the other routines, 
// and can do whatever initialization is needed.  
// The memory to be managed is passed into this routine.
void mem_init(unsigned char *my_memory, unsigned int my_mem_size){
  if (my_mem_size < min_size) my_mem_size = min_size;                // We need a bit of space for the freelist pointers 
  last = first = new_block(data_offset + my_mem_size);               // First bit of memory allocated
}

// A function functionally equivalent to malloc() , 
// but allocates it from the memory pool passed to mem_init().
void * my_malloc(size_t n) {
  // Find a free piece of memory:
  struct mem_block * cur = first;              // declaring a mem_block structure pointer called first to be set to first

  while (cur->occupied || cur->length < n) {   // while the current mem_block is occupied or the size of the block is smaller than n
    if (cur == last) break;                    // stop if we are at the last mem block
    cur = next_block(cur);                     // set the current mem block to the next mem block available
  }
  
  // if the current mem block is occupied
  if (cur->occupied) {
    // the last mem_block is set to the current mem block which is then set to the new mem block
    // No free piece, allocate new thing 
    last = cur = new_block(data_offset + n);
  } else {
    // otherwise, decalre a free mem block structure pointer called cur_free
    // and set it to a free mem block stucture pointer called cur
    struct free_mem_block * cur_free = (struct free_mem_block *) cur;
    cur_free->occupied = 1;

    if (data_offset + n + data_offset + min_size > cur_free->length) { // if we're trying to store more data then amount of free mem we have
      if (cur_free->prev_free)                                         // if there's a mem block before the current free block
	      cur_free->prev_free->next_free = cur_free->next_free;    // the prevs next one is the current one's next
      if (cur_free->next_free)                                         // if there's a next one 
	      cur_free->next_free->prev_free = cur_free->prev_free;    // set the prev of the next to the current's previous
      else                                                             // if there is no next one
	      last_free = cur_free->prev_free;                         // set last free to the previous to get rid of current free
    } 

    else {                                                                       // if we have enough mem space
      size_t rest_length = cur_free->length - data_offset - n;                   // current free amount of me length - the amount of mem length we're going to use = rest of mem length
      cur_free->length = data_offset + n;                                        // curent mem length = the length of the mem block + n_bytes
      struct free_mem_block * rest = (struct free_mem_block *) next_block(cur);  // declare free mem block structure pointer called rest, set it to point to the next block after cur
      rest->occupied = 0;                                                        // because we just declared it
      rest->length = rest_length;                                                // size of mem block
      rest->prev_free = cur_free->prev_free;                                     // set the prev of rest to the current's prev
      if (rest->prev_free)                                                       // if rest has a prev
	      rest->prev_free->next_free = rest;                                 // set the prev's next to the rest
      rest->next_free = cur_free->next_free;                                     // set rest's next to the current free's next
      if (rest->next_free)                                                       // if rest has a next
	      rest->next_free->prev_free = rest;                                 // set rest's next's prev to rest
      else                                                                       // if rest doesn't have a next
	      last_free = rest;                                                  // set the last free mem block to rest
      if (last == cur)                                                           // if the cur mem block is the last
	      last = (struct mem_block *) rest;                                  // set last to point to mem rest
    }
  }
  return cur->data;
}

// A function to merge 2 free mem blocks
void merge_blocks(struct free_mem_block * b1, struct free_mem_block * b2) {     // takes in pointers to 2 free mem blocks
  size_t new_length = b1->length + b2->length;                                  // combine the mem block sizes
  b1->next_free = b2->next_free;                                                // set the 1st mem block's next to the next one of the 2nd mem block
  if (b2->next_free)                                                            // if the 2nd mem block has a next
    b2->next_free->prev_free = b1;                                              // set the 2nd mem block's next's prev to the first mem block
  if (last_free == b2)                                                          // if the last mem block free is the 2nd mem block
    last_free = b1;                                                             // set the last free block to the 1st block
  if (last == (struct mem_block *) b2)                                          
    last = (struct mem_block *) b1;                   
  b1->length = new_length;                                                      // set the new length of the merged blocks the the length of the 1st block
}

// check if we can merge two blocks
void merge_check(struct free_mem_block * block) {
  char merge_prev = (block->prev_free && next_block(block->prev_free) == (struct mem_block *) block);
  char merge_next = (next_block(block) == (struct mem_block *) block->next_free);

  if (merge_prev) {
    merge_blocks(block->prev_free, block);
  }
  if (merge_next) {
    merge_blocks(block->prev_free, block->next_free);
  }
}


// A function equivalent to free() but returns the memory to the pool passed to mem_init().
void my_free(void * ptr) {
  struct free_mem_block * block = (struct free_mem_block *) (ptr - data_offset); // declare free_mem_block pointer called block

  // Find the next free block from here:
  struct mem_block * cur = (struct mem_block *) block; // declare mem_block pointer called cur
  while (cur != last && cur->occupied) {               // loop while we haven't reach the end and the current is occupied
    cur = next_block(cur);                             // set the cur to the next block
  }
  if (cur->occupied) {                                 // if it is occupied, we're at the last free block
    block->occupied = 0;
    block->next_free = NULL;                           // set the next block to NULL
    if (last_free) {
      last_free->next_free = block;                    // set the last free to this last block
    }
    block->prev_free = last_free;                      // set the prev free to the last free block
    last_free = block;                                 // set the last free to this block
    merge_check(block);                                // merge the 2 blocks together
    return;
  }
  
  // now set the block occupied to 0, cur is free 
  block->occupied = 0;                       
  struct free_mem_block * cur_free = (struct free_mem_block *) cur;
  
  // insert into freelist:
  block->next_free = cur_free;                  // set the block's next to be the cur_free block
  block->prev_free = cur_free->prev_free;       // set the block's prev to be the cur_free's prev_free
  if (cur_free->prev_free) {                    // if the cur_free has a pre_free
    cur_free->prev_free->next_free = block;     // set the next to that block
  }
  cur_free->prev_free = block;                  // set the cur's prev to that block 

  merge_check(block);                           // merge check the block
}

// Provides statistics about the current allocation of the memory pool
void memstats() {
#ifndef NDEBUG
  printf("Memory stats:\n");
  if (first == NULL) {
    printf("Nothing allocated\n");
  } else {
    struct mem_block * cur = first;
    for (;;) {
      if (cur->occupied) {
	printf("* %p %ld occupied\n", cur, cur->length);
      } else {
	struct free_mem_block * cur_free = (struct free_mem_block *) cur;
	printf("* %p %ld free %p %p\n", cur_free, cur_free->length, cur_free->next_free, cur_free->prev_free);
      }
      if (cur == last) break;
      cur = next_block(cur);
    }
  }
  printf("End memory stats\n");
#endif // NDEBUG
}

void memconsistency() {
#ifndef NDEBUG
  if (first == NULL) return;                 // if it's empty, return 
  struct mem_block * cur = first;            // declare mem_block structure pointer called cur that is set to first
  int saw_free = 0;
  struct free_mem_block * last_seen = NULL;  // declare free_mem_block structure pointer called last_seen set to NULL
  struct free_mem_block * expect = NULL;     // declare free_mem_block structure pointer called expect set to NULL
  char seen_any_free = 0;
  for (;;) {                                 // a continuous loop until a 'break'
    if (cur->occupied) {                     // if cur.occupied exists 
      saw_free = 0;                        
    } 
    else {                                   // if cur.occupied is NULL
      struct free_mem_block * cur_free = (struct free_mem_block *) cur; // declare a free_mem_clock pointer called cur_free set to a free_mem_block pointer called cur
      if (saw_free) { 
        printf("Memory consistency %p: Seen %d free blocks in a row\n", cur_free, saw_free);
      }
      if (cur_free->prev_free != last_seen) { 
        printf("Memory consistency %p: Expected prev_free = %p, got %p\n", cur_free, last_seen, cur_free->prev_free);
      }
      if (seen_any_free && cur_free != expect) {
        printf("Memory consistency %p: Expected %p to be the next free block\n", cur_free, expect);
      }
      ++saw_free; // increment 
      last_seen = cur_free;
      expect = cur_free->next_free;
      seen_any_free = 1;
    }
    if (cur == last) break;
    cur = next_block(cur);
  }
  if (last_seen != last_free) {
    printf("Memory consistency: Expected %p to be the last free block, got %p\n", last_free, last_seen);
  }
#endif // NDEBUG
}


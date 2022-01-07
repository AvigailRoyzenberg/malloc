# Memory Allocator
mM implementation of malloc() and free()

My memory allocator uses a linked list structure to keep track of free or occupied memory blocks.

It has a few basic functions:
- `mem_init()`
- `my_malloc()`
- `mem_free()` which uses:
    -  `merge_check()`and `merge_blocks()` 
-  `mem_stats()`

There are several rrror / return codes / exceptions handlings but mainly in `test_mem_consistency()`

The `main()` is in the `test.c` file

To use the makefile to compile and run, download the repo and in the command prompt run:
`make && test./`

To edit, recompile, and run again, in the command prompt run:
`make clean`
`make && ./test`

#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
//#include "malloc.c"

/*The list element (memory block)*/
typedef union header
{
	struct
	{
		size_t size; //the size in 
		union header *next;
	} block;
	struct
	{
		long ar[2];
	} _align;
} Header;

void *our_malloc(size_t);
void *our_realloc(void *,size_t);
/* Explicit function declarations */
void init_header(Header *, int);
void our_free(void *);
void *extend_heap(int);
void *find_block(int);
void print_list(void);
void print_lists(void);
void *fit123(int);
void *first_fit(int);
void *best_fit(int);
void *worst_fit(int);
void *quick_fit(int);
int select_list(int);
void *use_block(Header *, Header *, int);
void insert_to_list(Header *);
void merge_blocks(Header *, Header *);
void init(void);
int ql_size(int);






#define HEADERSIZE sizeof(Header)
#define MINSIZE 512 //Allocates this amount of memory or more for most strategies
#define nunits(x) (((x) + HEADERSIZE -1) / HEADERSIZE + 1) //Each block an integer multiple of headers
/*
Strategies:
1 = first fit
2 = best fit
3 = worst fit
4 = quick fit
*/
#define STRATEGY 1
#define NRQUICKLISTS 10
#define BLOCKS_TO_ALLOCATE 10
#define DB 1

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

void *base = NULL; //Base of the common list for f-fit, b-fit and w-fit
void *ql[NRQUICKLISTS]; //The quick-lists bases for q-fit
void *end_heap = 0; //The adress at the end of the heap


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

/*
The init() function initiates global variables
The init() function takes no parameters and returns no value
*/
void init(void)
{
	if(DB)
		printf("RUNNING %s \n", __FUNCTION__);
	int i = 0;
	if(STRATEGY == 4)
	{
		for(; i<NRQUICKLISTS; i++)
		{
		ql[i] = NULL;
		}
	}
	end_heap = sbrk(0);
}

/*
Returns a pointer to an allocated memory block of size "size".
The our_malloc() function extends the heap if necessary.
If memory cannot be allocated, or if the specified size is zero,
the our_malloc() function returns NULL.
*/
void *our_malloc(size_t size)
{
	if(size==0)		//Returns null-pointer if size is zero
		return (void *) NULL;
	if(!end_heap) //if malloc is called for the first time
		init();

	void *p = find_block(nunits(size));	// finds a block of sufficient size using predefined allocation strategy
	if(p != (void *) -1)
		init_header((Header *)p, nunits(size));
	return (void *)((Header *)p + 1); //returns the pointer to the address right after the header
}

/*
The our_realloc() function takes a pointer "p" and a size "size".
If "size" is equal to, or one header smaller than the memory block pointed to by "p",
p is returned unchanged.

If "size" more than one smaller than the memory block pointed to by "p",
the our_realloc() function decreases the size of the memory block, creates a new free memory
block from the remaining memory, and returns p unchanged.

If "size" is larger than the current memory block, the our_malloc() function allocates the
requested amount of memory, copies any data from the old block to the new, and frees the old memory block.
*/
void *our_realloc(void *p, size_t size)
{
	Header *src_header = (Header *)p-1;
	int diff = (int)(src_header->block.size) - nunits(size);
	void *q = p;
	if(diff > 1)
	{
		src_header->block.size = nunits(size);
		Header *new_block = src_header + src_header->block.size;
		init_header(new_block, diff);
		our_free(new_block+1);
	}
	else if(diff < 0)
	{
		q = our_malloc(size);
		memcpy(q, p, (src_header->block.size-1)*HEADERSIZE);
		our_free(p);
	}
	return q;
}

/*
The init_header() function initiates a header to the block pointed to by "hp" with with a size of "nunits"*HEADERSIZE.
It returns no value.
*/
void init_header(Header * hp, int nunits)
{
	hp->block.size = (size_t) nunits;
}

/*
The find_block() function calls the appropriate helper function depending on strategy.
*/
void *find_block(int nunits)
{
	void *p = NULL;

	switch(STRATEGY)
	{
		case 4:
			p = quick_fit(nunits);
			break;
		default:
			p = fit123(nunits);
	}
	return p;
}

/*The fit123() calls the extend_heap function if there are no memory blocks in the
free-list, or if no block of sufficient size has been found in the free-list, after a call to the appropriate
strategy function, for strategies
1, 2 and 3.*/
void * fit123(int nunits)
{
	void *p = NULL;
	if(!base) //list is empty
		p = extend_heap(nunits);
	else
	{
		switch(STRATEGY)
		{
			case 1:
				p = first_fit(nunits);
				break;
			case 2:
				p = best_fit(nunits);
				break;
			case 3:
				p = worst_fit(nunits);
		}
		if(!p)
			p = extend_heap(nunits);
	}
	return p;
}

/*The first_fit() function returns a pointer to the first encountered memory block of sufficient size*/
void *first_fit(int nunits)
{
	if(DB)
		printf("RUNNING %s \n", __FUNCTION__);
	void *p;

	Header * current = base, * previous = current;
	while(current) // as long as the end of the list has not been reached
	{
		if(current->block.size >= nunits) //if block is big enough...
		{
			p = use_block(current, previous, nunits);
			break;
		}
		previous = current;	//move to next element
		current = current->block.next;
	}
	if(!current) //reached end of list without finding sufficient block
		p = NULL;
	return p;
}

/*The best_fit() function returns a pointer to the memory block that best matches the requested size*/
void *best_fit(int nunits)
{
	if(DB)
		printf("RUNNING %s \n", __FUNCTION__);
	long diff = -1;
	void *p = NULL, *pre_p = NULL;

	Header *current = base, *previous = current;

	while(current)
	{
		if((int)(current->block.size)-nunits>-1 ) //block sufficient size
		{
			if(diff == -1 || (int)(current->block.size)-nunits < diff)
			{

				diff = (int)(current->block.size)-nunits;
				p = current;
				pre_p = previous;
				if(!diff)
					break;
			}
		}
		previous = current;
		current = current->block.next;
	}
	if(p)
		p = use_block(p, pre_p, nunits);
	return p;
}

/*The best_fit() function returns a pointer to the memory block that least matches the requested size*/
void *worst_fit(int nunits)
{
	if(DB)
		printf("RUNNING %s \n", __FUNCTION__);
	long diff = -1;
	void *p = NULL, *pre_p = NULL;

	Header *current = base, *previous = current;

	while(current)
	{
		if((int)(current->block.size)-nunits>-1 ) //block sufficient size
		{
			if((int)(current->block.size)-nunits > diff)
			{
				diff = (int)(current->block.size)-nunits;
				p = current;
				pre_p = previous;
				if(!diff)
					break;
			}
		}
		previous = current;
		current = current->block.next;
	}
	if(p)
		p = use_block(p, pre_p, nunits);
	return p;
}
/*The quick_fit() function keeps quick-lists with memory blocks of 16, 32, ..., 2^(4+NRQUICKLISTS) bytes.*/
void *quick_fit(int nunits)
{
	if(DB)
		printf("RUNNING %s \n", __FUNCTION__);
	void *base_copy = base, *p = NULL; //Välj lista (utifrån storlekar)
	int list_index = select_list(nunits);
	if(ql_size(list_index))
	{
		if(DB)
			printf("	Adjusting size from %lu to %lu\n", (nunits-1)*HEADERSIZE,(ql_size(list_index)-1)*HEADERSIZE);
		nunits = ql_size(list_index);
	}

	base = ql[list_index];

	if(!base) //list is empty
		p = extend_heap(nunits);
	else
	{
		p = first_fit(nunits);
		ql[list_index]= base;
	}
		
	base = base_copy;
	return p;
}

int select_list(int nunits)
{
	if(DB)
		printf("RUNNING %s \n", __FUNCTION__);
	int i = 0;

	while(ql_size(i))
	{
		if(nunits <= ql_size(i))
			break;
		i++;
	}
	return i;
}

int ql_size(int index)
{
	if(index == NRQUICKLISTS-1)
		return 0;
	else
		return nunits(16 << index);
}


void *use_block(Header *b, Header *previous, int nunits)
{
	if(DB)
		printf("RUNNING %s \n", __FUNCTION__);
	void *p;
	if (b->block.size == nunits) //... and is exactly as big as specified
	{
		if(DB)
			printf("BLOCK REMOVED FROM FREE-LIST\n");


		p = b;
		if(b == base) //if it was the first element, move base to the next element
		{
			if(DB)
				printf("	BASE REMOVED\n");
			base = b->block.next;
		}
		else
		{
			if(DB)
				printf("	BASE KEPT\n");
			if(DB)
				printf("	Previous: %p b: %p Next: %p\n", previous,b, b->block.next);
			previous->block.next = b->block.next; // link the predecessor element to the successor element
		}
	}
	else //and is bigger than necessary
	{
		if(DB)
			printf("BLOCK SPLIT\n");

		b->block.size -= nunits; //chop off end of block
		p = b + b->block.size; //select the chopped off part
	}
	return p;
}

void *extend_heap(int nunits)
{
	if(DB)
		printf("RUNNING %s \n", __FUNCTION__);
	Header *p, *q;

	if(STRATEGY == 4 && nunits <= ql_size(NRQUICKLISTS-2))
	{
		if(DB)
			printf("	quick-fit-mode\n");
		p = (Header *) mmap(end_heap, (size_t) (BLOCKS_TO_ALLOCATE * nunits * HEADERSIZE), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
		if(p != (void *) -1)
		{
			if(DB)
				printf("	allocation OK\n");
			q = p;
			int i = 0;
			for(; i<BLOCKS_TO_ALLOCATE; i++)
			{
				if(DB)
					printf("	writing header: %d\n",nunits);
				init_header(q, nunits);
				if(i)
					our_free(q+1);
				q+= nunits;
			}
		}
		else
		{
			if(DB)
				printf("	allocation not OK\n");
			p = (Header *) mmap(end_heap, (size_t) (nunits * HEADERSIZE), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
		}

	}

	else if(nunits < nunits(MINSIZE))
	{
		p = (Header *) mmap(end_heap, (size_t) ((nunits(MINSIZE) + 1) * HEADERSIZE), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
		if(p != (void *) -1)
		{
			init_header(p, nunits(MINSIZE)+1);
			p->block.size -= nunits; //chop off end of block
			our_free(p+1);
			p = p + p->block.size;
			init_header(p, nunits);
		}
		else
			p = (Header *) mmap(end_heap, (size_t) (nunits * HEADERSIZE), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
	}
	else
	{
		p = (Header *) mmap(end_heap, (size_t) (nunits * HEADERSIZE), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
	}
	end_heap = sbrk(0);
	return p;
}

void our_free(void *vp)
{
	if(DB)
		printf("RUNNING %s \n", __FUNCTION__);
	Header *p = (Header *)vp - 1; //p points at header
	if(STRATEGY==4)
	{
		int list_index = select_list(p->block.size);
		if(DB)
		{
			// printf("	quick-fit-mode\n");
			// printf("	Block size (excl. header): %lu\n", (p->block.size-1)*HEADERSIZE);
			// printf("	List index: %d\n", list_index);
		}
		void *base_copy = base;
		base = ql[list_index];
		insert_to_list(p);
		ql[list_index] = base;
		base = base_copy;
	}
	else
		insert_to_list(p);
}

void insert_to_list(Header *p)
{
	if(DB)
		printf("RUNNING %s \n", __FUNCTION__);
	Header *previous = base, *current = base;
	while(1)
	{
		if (current == NULL) //empty list
			break;
		else if (p == current)
			printf("ERROR!!! - Already in list\n");
		else if (p > current) //block is to be placed further down the list (sorted by ascending address)
		{
			previous = current;
			current = current->block.next;
		}
		else
			break;
	}
	
	if(base == NULL || current == base) //either the list is empty or p is the smaller than the first address and should be placed first
	{
		p->block.next = base;
		base = p;
	}
	else //p is placed somewhere in the middle of the list
	{
		p->block.next = current;
		previous->block.next = p;
	}

	if(STRATEGY != 4)
		merge_blocks(previous, p);
}

void merge_blocks(Header *previous, Header *p)
{
	if(DB)
		printf("RUNNING %s \n", __FUNCTION__);
	if(p != base && previous + previous->block.size == p) //merges with preceeding block if possible
	{
		if(DB)
			printf("	merging w previous\n");
		
		previous->block.size += p->block.size;
		previous->block.next = p->block.next;
		p = previous;
	}

	if(p->block.next != (Header *) NULL && p + p->block.size == p->block.next) //merges with succeeding block if possible
	{	
		if(DB)
			printf("	merging w next\n");
		p->block.size += p->block.next->block.size;
		p->block.next = p->block.next->block.next;
	}	
}


void print_list()
{
	Header *current = base;
	int i = 1;
	while(current)
	{
		printf("%d: %p, size: %lu next: %p\n",i, current+1, current->block.size*HEADERSIZE,current->block.next+1);
		current = current->block.next;
		i++;
	}
	if(i==1)
		printf("Empty list!\n");
	printf("\n\n");
}

void print_lists()
{
	void *base_copy = base;
	int i=0;
	for(; i<NRQUICKLISTS; i++)
	{
		base = ql[i];
		printf("Size %lu:\n", (ql_size(i)-1)*HEADERSIZE);
		print_list();
	}
	base = base_copy;
}

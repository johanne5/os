
#define HEADERSIZE sizeof(Header)
//#define MINSIZE 4096
#define MINSIZE 0
#define nunits(x) (((x) + HEADERSIZE -1) / HEADERSIZE + 1)
#define STRATEGY 3

#define NLISTS 7
#define BLOCKS_TO_ALLOCATE 10

typedef union header
{
	struct
	{
		size_t size;
		union header *next;
	} block;
	struct
	{
		long ar[2];
	} _align;
} Header;

void *base = NULL;

void *bases[7] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL};
int list_size[7] = {nunits(8), nunits(16), nunits(32), nunits(64), nunits(128), nunits(256), nunits(0)};

void init_header(Header *, int);
void *extend_heap(int);
void *find_block(int);
void our_free(void *);
void print_list(void);
void *fit123(int);
void *first_fit(int);
void *best_fit(int);
void *worst_fit(int);
void *quick_fit(int);
int select_list(int);
void *use_block(Header *, Header *, int);
void insert_to_list(Header *);
void merge_blocks(Header *, Header *);

/*
Returns an allocated memory block of the specified size size.
Extends the heap if necessary
*/
void *our_malloc(size_t size)
{
	if(size==0)		//Returns null-pointer if size is zero
		return (void *) NULL;
	void *p = find_block(nunits(size));	// finds a block of sufficient size using predefined allocation strategy
	if(p != (void *) -1)
		init_header((Header *)p, nunits(size));
	return (void *)((Header *)p + 1); //returns the pointer to the address right after the header
}

void init_header(Header * hp, int nunits)
{
	hp->block.size = (size_t) nunits;
}

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

void *first_fit(int nunits)
{
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

void *best_fit(int nunits)
{
	long diff = -1;
	void *p = NULL;

	Header *current = base, *previous = current;

	while(current)
	{
		if((int)(current->block.size)-nunits>-1 ) //block sufficient size
		{
			if(diff == -1 || (int)(current->block.size)-nunits < diff)
			{

				diff = (int)(current->block.size)-nunits;
				p = current;
			}
		}
		previous = current;
		current = current->block.next;
	}
	if(p)
		p = use_block(p, previous, nunits);
	return p;
}

void *worst_fit(int nunits)
{
	long diff = -1;
	void *p = NULL;

	Header *current = base, *previous = current;

	while(current)
	{
		if((int)(current->block.size)-nunits>-1 ) //block sufficient size
		{
			if((int)(current->block.size)-nunits > diff)
			{
				diff = (int)(current->block.size)-nunits;
				p = current;
			}
		}
		previous = current;
		current = current->block.next;
	}
	if(p)
		p = use_block(p, previous, nunits);
	return p;
}

void *quick_fit(int nunits)
{
	void *base_copy = base, *p = NULL; //Välj lista (utifrån storlekar)
	int list_index = select_list(nunits);
	nunits = list_size[list_index];
	base = bases[list_index];

	if(!base) //list is empty
		p = extend_heap(nunits);
	else
	{
		p = first_fit(nunits);
		if(!p)
			p = extend_heap(nunits);
	}
	base = base_copy;
	return p;
}

int select_list(int nunits)
{
	int i = 0;

	while(list_size[i])
	{
		if(nunits <= list_size[i])
			break;
		i++;
	}
	return i;
}


void *use_block(Header *b, Header *previous, int nunits)
{
	//printf("this happens\n");
	//printf("current : %p\nprevious: %p\n",b,previous);
	void *p;
	if (b->block.size == nunits) //... and is exactly as big as specified
	{


		p = b;
		if(b == base) //if it was the first element, move base to the next element
			base = b->block.next;
		else
			previous->block.next = b->block.next; // link the predecessor element to the successor element
	}
	else //and is bigger than necessary
	{

		b->block.size -= nunits; //chop off end of block
		p = b + b->block.size; //select the chopped off part
	}
	return p;
}

void *extend_heap(int nunits)
{
	Header *p, *q;

	if(STRATEGY == 4) {
		p = (Header *) mmap(NULL, (size_t) (BLOCKS_TO_ALLOCATE * nunits * HEADERSIZE), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if(p != (void *) -1)
		{
			q = p;
			int i = 0;
			for(; i<BLOCKS_TO_ALLOCATE; i++)
			{
				init_header(q, nunits);
				if(i)
					free(q+1);
			}
		}
		else
		{
			p = (Header *) mmap(NULL, (size_t) (nunits * HEADERSIZE), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		}

		return p;
	}

	if(nunits + 1 < nunits(MINSIZE))
	{
		p = (Header *) mmap(NULL, (size_t) (nunits(MINSIZE) * HEADERSIZE), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if(p != (void *) -1)
		{
			init_header(p, nunits(MINSIZE));
			p->block.size -= nunits; //chop off end of block
			our_free(p+1);
			p = p + p->block.size;
		}
		else
			p = (Header *) mmap(NULL, (size_t) (nunits * HEADERSIZE), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	}
	else
	{
		p = (Header *) mmap(NULL, (size_t) (nunits * HEADERSIZE), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	}
	return p;
}

void our_free(void *vp)
{
	Header *p = (Header *)vp - 1; //p points at header
	insert_to_list(p);
}

void insert_to_list(Header *p)
{
	Header *previous = base, *current = base;
	while(1)
	{
		if (current == NULL) //empty list
			break;
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

	if(p != base && previous + previous->block.size == p) //merges with preceeding block if possible
	{
		printf("merging w previous\n");
		previous->block.size += p->block.size;
		previous->block.next = p->block.next;
		p = previous;
	}

	if(p->block.next != (Header *) NULL && p + p->block.size == p->block.next) //merges with succeeding block if possible
	{	
		printf("merging w next\n");
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


#define HEADERSIZE sizeof(Header)
#define MINSIZE 4096
#define nunits(x) (((x) + HEADERSIZE -1) / HEADERSIZE + 1)
#define STRATEGY 2

void *base = NULL;

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

void init_header(Header *, int);
void *extend_heap(int);
void *find_block(int);
void our_free(void *);
//void print_list(void);
void *first_fit(int);
void *best_worst_fit(int);
void *use_block(Header *, Header *, int);
Header *insert_to_list(Header *);
void merge_blocks(Header *);

/*
Returns an allocated memory block of the specified size size.
Extends the heap if necessary
*/
void *our_malloc(size_t size)
{
	if(size==0)		//Returns null-pointer if size is zero
		return (void *) NULL;
	void *p = find_block(nunits(size));	// finds a block of sufficient size using predefined allocation strategy
	init_header((Header *) p, nunits(size)); //writing the header of the memory block
	return (void *)((Header *)p + 1); //returns the pointer to the address right after the header
}

void init_header(Header * hp, int nunits)
{
	hp->block.size = (size_t) nunits;
}

void *find_block(int nunits)
{
	void *p;

	switch(STRATEGY)
	{
		case 1:
			p = first_fit(nunits);
			break;
		case 2: //best fit
			p = best_worst_fit(nunits);
			break;
		case 3: //worst fit
			p = best_worst_fit(nunits);
			break;
		case 4:
			break;
	}
	return p;
}

void *best_worst_fit(int nunits)
{
	long diff = -1;
	void *p = (void *) NULL;
	Header *current = (Header *) base, *previous = current;
	while(current)
	{
		if(current->block.size-nunits > -1) //block sufficient size
		{
			if(STRATEGY == 2)
			{
				if(diff == -1 || current->block.size-nunits < diff)
				{
					diff = current->block.size-nunits;
					p = current;
				}
			}

			else if(STRATEGY == 3)
			{
				if(current->block.size-nunits > diff)
				{
					diff = current->block.size-nunits;
					p = current;
				}
			}
		}
		previous = current;
		current = current->block.next;
	}



	if(!p)
		p = extend_heap(nunits);
	else
		p = use_block(current, previous, nunits);
	return p;
}

void *first_fit(int nunits)
{
	void *p;

	if(base) //if the free-list contains any element
	{
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
			p = extend_heap(nunits);
	}
	else //list is empty
		p = extend_heap(nunits);
	return p;
}

void *use_block(Header *b, Header *previous, int nunits)
{
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
	//nunits = nunits < nunits(MINSIZE) ? nunits(MINSIZE) : nunits;
	
	Header *p;
	
	if(nunits < nunits(MINSIZE))
	{
		p = (Header *) mmap(NULL, (size_t) (nunits(MINSIZE) * HEADERSIZE), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		init_header(p, nunits(MINSIZE));
		p->block.size -= nunits; //chop off end of block
		insert_to_list(p);
		p = p + p->block.size;
	}
	else
	{
		p = (Header *) mmap(NULL, (size_t) (nunits * HEADERSIZE), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		init_header(p, nunits);
	}	
	return p;
}

Header *insert_to_list(Header *p)
{
	Header *previous = base, *current = base;
	while(1)
	{
		if (current == (Header *) NULL) //empty list
			break;
		else if (p > current) //block is to be placed further down the list (sorted by ascending address)
		{
			previous = current;
			current = current->block.next;
		}
		else
			break;
	}
	
	if(base == (void *) NULL || current == base) //either the list is empty or p is the smaller than the first address and should be placed first
	{
		p->block.next = base;
		base = p;
	}
	else //p is placed somewhere in the middle of the list
	{
		p->block.next = current;
		previous->block.next = p;
	}
	return previous;
}

void merge_blocks(Header *previous)
{
	Header *p = previous->block.next;
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

void our_free(void *vp)
{
	Header *p = (Header *)vp - 1; //p points at header
	Header *previous = insert_to_list(p);
	merge_blocks(previous);
	
	
}

void print_list()
{
	Header *current = (Header *) base;
	while(current)
	{
		printf("%p\n", current+1);
		current = current->block.next;
	}
	printf("\n\n");
}

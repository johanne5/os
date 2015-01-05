
#define HEADERSIZE sizeof(Header)
#define MINSIZE 4096
#define nunits(x) (((x) + HEADERSIZE -1) / HEADERSIZE + 1)

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

void *our_malloc(size_t size)
{
	if(size==0)
		return (void *) NULL;

	void *p = find_block(nunits(size));
	// printf("%p\n", p);

	init_header((Header *) p, nunits(size));
	//printf("%d\n",*(int *)p);

	// printf("%p\n", (void *)((Header *)p + 1));

	return (void *)((Header *)p + 1);
}

void init_header(Header * hp, int nunits)
{
	hp->block.size = (size_t) nunits;
}

void *find_block(int nunits)
{
	void *p;

	if(base)
	{
		Header * current = base, * previous = current;
		while(current)
		{
			if(current->block.size >= nunits)
			{
				if (current->block.size == nunits)
				{
					printf("Iraq\n");
					//dont forget to remove from free list
					p = current;
					if(current == previous)
						base = current->block.next;
					else
						previous->block.next = current->block.next;
					break;
				}
				else
				{
					printf("Iran\n");
					current->block.size -= nunits;
					p = current + current->block.size;
					break;
				}
			}
			previous = current;
			current = current->block.next;
		}

		if(!current)
		{
			printf("EOL\n");
			p = extend_heap(nunits);
		}

	}
	else //listan är tom
	{
		printf("EL\n");
		p = extend_heap(nunits);
	}
	return p;
}

void *extend_heap(int nunits)
{
	return (void *) mmap(NULL, (size_t) (nunits * HEADERSIZE), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

void our_free(void *vp)
{
	Header *p = (Header *)vp - 1;
	p->block.next = (Header *) base;
	base = p;

	Header * previous = base, * current = base;

	while((Header *) p > current->block.next)
	{
		printf("%p\n", base);
		printf("%p\n", current);
		printf("%p\n", current->block.next);
		printf("%p\n", p);
		break;
		if(!current->block.next)
			break;
		previous = current;
		current = current->block.next;
	}
	if(current==base)
	{
		((Header *)p)->block.next = base;
		base = p;
	}
	else
	{
		p->block.next = current->block.next;
		previous->block.next = p;
	}

	if(previous + previous->block.size == (void *) p - 1)
	{
		previous->block.size += p->block.size;
		previous->block.next = p->block.next;
		p = previous;
	}
	if(p + p->block.size == (void *) p->block.next -1)
	{
		p->block.size += p->block.next->block.size;
		p->block.next = p->block.next->block.next;
	}
	printf("Freeman was here\n");
		
}

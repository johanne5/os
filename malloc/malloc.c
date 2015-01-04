void *base = NULL;

void *our_malloc(size_t size)
{
	typedef struct s_block *t_block;

	struct s_block {
		size_t size;
		t_block next;
		int free;
	};

	
	int metasize = sizeof(struct s_block);

	
	if(base)
	{
		void *p;
		p = mmap(base, size+metasize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0 );
		p[0] = *t_block{.size = size, .next = NULL, .free=0};
	}
	else
	{
		void *p;
		p = mmap(base, size+metasize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0 );
		p[0] = *t_block{.size = size, .next = NULL, .free=0};
		base = p;
	}

	return p+metasize;


}

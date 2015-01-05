#include "malloc.h"

int main(int argc, char *argv[])
{
	void *p = our_malloc((size_t) 240);
	// printf("%p\n", p);

	
	our_free(p);



	void *p1, *p2;
	p1 = our_malloc((size_t) 112);
	// printf("%p\n", p);
	p2 = our_malloc((size_t) 112);
	// printf("%p\n", p);
	our_free(p1);
	our_free(p2);

	p = our_malloc((size_t) 240);
	our_free(p);


	return 0;
}
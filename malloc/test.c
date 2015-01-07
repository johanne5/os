#include "malloc.h"

int main(int argc, char *argv[])
{
	void *p0, *p1, *p2;

	p0 = our_malloc((size_t) 64);
	printf("Iran\n");
	// // printf("%p\n", p);

	print_list();
	our_free(p0);

	print_list();





	
	p1 = our_malloc((size_t) 32);
	

	print_list();

	// printf("%p\n", p);

	p2 = our_malloc((size_t) 32);
	print_list();
	// printf("%p\n", p);
	// p3 = our_malloc((size_t) 112);
	//printf("p0: %p\np1: %p\np2: %p\n\n", p0, p1, p2 );
	

	our_free(p2);

	
	print_list();
	
	//printf("hello\n");
	our_free(p1);

	print_list();
	
	
	// our_free(p3);

	p0 = our_malloc((size_t) 64);

	print_list();
		
	our_free(p0);
	print_list();

	return 0;
}
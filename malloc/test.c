#include "malloc.h"

int main(int argc, char *argv[])
{
	void *p0;//, *p1, *p2;

	p0 = our_malloc((size_t) 4096);
	our_free(p0);
	p0 = our_malloc((size_t) 1024);

	print_list();

	p0 = our_realloc(p0,2048);

	print_list();
	our_free(p0);

	print_list();





	
	// p1 = our_malloc((size_t) 32);
	

	// print_lists();

	// printf("%p\n", p);

	// p2 = our_malloc((size_t) 32);
	// print_lists();
	// printf("%p\n", p);
	// p3 = our_malloc((size_t) 112);
	//printf("p0: %p\np1: %p\np2: %p\n\n", p0, p1, p2 );
	

	// our_free(p2);
	// printf("SOMETHING WENT WRONG BEFORE THIS POINT!!!!!!\n");

	
	//print_lists();
	// 
	//printf("hello\n");
	// our_free(p1);

	// print_lists();
	
	
	// our_free(p3);

	// p0 = our_malloc((size_t) 64);
	// print_lists();
		
	// our_free(p0);
	// print_lists();

	return 0;
}
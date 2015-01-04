#include "malloc.h"

int main(int argc, char *argv[])
{
	//printf("this happens\n");
	int* p = our_malloc((size_t) sizeof(int));
	*p=42;
	printf("%p %d\n", p,*p);
	munmap(p, (size_t) sizeof(int));

	return 0;
}
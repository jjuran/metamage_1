/*
	hello.c
	-------
*/

// Standard C
#include <stdio.h>


extern const char* world;

int main()
{
	printf( "Hello %s\n", world );
	
	return 0;
}

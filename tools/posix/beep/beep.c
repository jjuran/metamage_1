/*
	beep.c
	------
*/

// must
#include "must/write.h"


#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif


int main( void )
{
	const char c = 0x07;
	
	must_write( STDOUT_FILENO, &c, sizeof c );
	
	return 0;
}


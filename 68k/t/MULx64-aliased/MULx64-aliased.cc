/*
	MULx64-aliased.cc
	-----------------
*/

// POSIX
#include <unistd.h>


#pragma exceptions off


#define STR_LEN( s )  "" s, (sizeof s - 1)

#define PRINT( s )  write( STDOUT_FILENO, STR_LEN( s "\n" ) )


#ifdef __MC68K__

/*
	According to Motorola documentation, behavior in this case is undefined.
	However, there's consensus among implementations to return the low half.
*/

static
asm
unsigned long multiply()
{
	machine 68020
	
	MOVEQ   #123,D0
	MOVEQ   #99,D1
	
	MULU.L  D1,D0:D0
	
	RTS
}

#else

static inline
unsigned long multiply()
{
	return 123 * 99;
}

#endif

int main( int argc, char** argv )
{
	PRINT( "1..1" );
	
	unsigned long product = multiply();
	
	if ( product == 12177 )
	{
		PRINT( "ok 1" );
	}
	else
	{
		PRINT( "not ok 1" );
	}
	
	return 0;
}

/*
	ccinfo.cc
	---------
*/

// Standard C
#include <stdio.h>


#define PRINT( format, name )  printf( "%s: %" #format "\n", #name, name )

int main( int argc, char** argv )
{
#ifdef __cplusplus
	PRINT( ld, __cplusplus );
#endif
	
#ifdef __LP64__
	PRINT( d, __LP64__ );
#endif
	
#ifdef __MWERKS__
	PRINT( x, __MWERKS__ );
#endif
	
#ifdef __GNUC__
	PRINT( d, __GNUC__ );
#endif
	
#ifdef __clang__
	PRINT( d, __clang__ );
#endif
	
	return 0;
}

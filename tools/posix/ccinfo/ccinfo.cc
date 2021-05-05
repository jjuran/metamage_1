/*
	ccinfo.cc
	---------
*/

// Standard C
#include <stdio.h>

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifdef __MACOS__
#include <ConditionalMacros.h>
#endif

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
	
#ifdef UNIVERSAL_INTERFACES_VERSION
	PRINT( x, UNIVERSAL_INTERFACES_VERSION );
#endif
	
	return 0;
}

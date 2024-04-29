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
	
#ifdef __BYTE_ORDER
	PRINT( d, __BYTE_ORDER );
#endif
	
#ifdef __BIG_ENDIAN
	PRINT( d, __BIG_ENDIAN );
#endif
	
#ifdef __LITTLE_ENDIAN
	PRINT( d, __LITTLE_ENDIAN );
#endif
	
#ifdef _BIG_ENDIAN
	PRINT( d, _BIG_ENDIAN );
#endif
	
#ifdef _LITTLE_ENDIAN
	PRINT( d, _LITTLE_ENDIAN );
#endif
	
#ifdef BYTE_ORDER
	PRINT( d, BYTE_ORDER );
#endif
	
#ifdef BIG_ENDIAN
	PRINT( d, BIG_ENDIAN );
#endif
	
#ifdef LITTLE_ENDIAN
	PRINT( d, LITTLE_ENDIAN );
#endif
	
#ifdef __MSL__
	PRINT( x, __MSL__ );
#endif
	
#ifdef UNIVERSAL_INTERFACES_VERSION
	PRINT( x, UNIVERSAL_INTERFACES_VERSION );
#endif
	
	return 0;
}

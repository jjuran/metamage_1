/*
	load.cc
	-------
*/

// Relix
#include "tool-runtime/parameter_block.h"

// Standard C
#include <errno.h>

#ifndef NULL
#define NULL  0L
#endif

static asm void* load( const char* path : __A0 ) : __A0
{
	MOVEA.L  A0,A1    ; // copy A0
	
	// ENTER strlen0
loop:
	TST.B    (A1)+    ;  // while ( *a1++ != '\0' )
	BNE.S    loop     ;  //    continue;
	
	SUBA     A0,A1    ;  // a1 -= a0;
	MOVE.L   A1,D0    ;  // d0 = a1;
	// LEAVE strlen0
	
	JSR  0xFFFFFFFC   ;  // the actual load callback
	
	MOVE.L   A0,D0    ;  // if ( *a0 != NULL ) ;
	BNE.S    no_errno ;  // else
	MOVE.L   D1,errno ;  //     errno = d1;
no_errno:
	RTS
}

typedef int (*main_entry)(int, char**, char**, void*);

int main( int argc, char** argv )
{
	if ( argc < 2 )
	{
		return 0;
	}
	
	const char* path = argv[1];
	
	void* program = load( path );
	
	if ( program == NULL )
	{
		return errno == ENOENT ? 127 : 126;
	}
	
	main_entry next = (main_entry) program;
	
	--argc;
	++argv;
	
	return next( argc, argv, NULL, global_system_params );
}


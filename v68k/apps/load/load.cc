/*
	load.cc
	-------
*/

// Relix
#include "tool-runtime/parameter_block.h"

// Standard C
#include <errno.h>

// v68k-utils
#include "utils/load.hh"

#ifndef NULL
#define NULL  0L
#endif

typedef int (*main_entry)(int, char**, char**, void*);

int main( int argc, char** argv )
{
	using v68k::utils::load;
	
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

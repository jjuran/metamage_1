/*
	realpath.cc
	-----------
*/

#include "vxo-posix/realpath.hh"

#ifdef __RELIX__
// POSIX
#include <unistd.h>
#else
// Standard C
#include <stdlib.h>
#endif

#include <errno.h>

// vxo
#include "vxo/errno.hh"

// plus
#include "plus/string.hh"

// vxo-string
#include "vxo-string/string.hh"


#pragma exceptions off


namespace vxo
{

Box realpath( const char* path )
{
	plus::string result;
	
#ifdef __RELIX__
	
	ssize_t size = vxo::datum_max_offset;
	
	while ( size != result.size() )
	{
		char* p = result.reset_nothrow( size );
		
		if ( p == NULL )
		{
			return Error( out_of_memory );
		}
		
		size = _realpath( path, p, result.size() );
		
		if ( size < 0 )
		{
			if ( size == -1 )
			{
				return Errno( errno );
			}
			
			size = ~size;
		}
	}
	
#else
	
	char* alloc = ::realpath( path, NULL );
	
	if ( alloc == NULL )
	{
		return Errno( errno );
	}
	
	result.assign( alloc, strlen( alloc ), vxo::delete_free );
	
#endif
	
	return String( result );
}

}

/*
	read.cc
	-------
*/

#include "vfs/filehandle/primitives/read.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/filehandle/methods/bstore_method_set.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/stream_method_set.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	ssize_t read( filehandle& that, char* buffer, size_t n )
	{
		if ( const filehandle_method_set* methods = that.methods() )
		{
			if ( const stream_method_set* stream_methods = methods->stream_methods )
			{
				if ( stream_methods->read )
				{
					return stream_methods->read( &that, buffer, n );
				}
				
				p7::throw_errno( EPERM );
			}
			
			if ( const bstore_method_set* bstore_methods = methods->bstore_methods )
			{
				if ( bstore_methods->pread )
				{
					ssize_t n_read = bstore_methods->pread( &that, buffer, n, that.get_mark() );
					
					return that.advance_mark( n_read );
				}
			}
		}
		
		p7::throw_errno( EPERM );
		
		// Not reached
		return 0;
	}
	
}


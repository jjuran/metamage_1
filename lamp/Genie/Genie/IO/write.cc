/*
	write.cc
	--------
*/

#include "vfs/filehandle/primitives/write.hh"

// POSIX
#include <fcntl.h>

// vfs
#include "vfs/filehandle.hh"
#include "vfs/filehandle/methods/bstore_method_set.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/stream_method_set.hh"
#include "vfs/filehandle/primitives/append.hh"

// Genie
#include "Genie/IO/Stream.hh"


namespace vfs
{
	
	using namespace Genie;
	
	
	ssize_t write( filehandle& that, const char* buffer, size_t n )
	{
		if ( const filehandle_method_set* methods = that.methods() )
		{
			if ( const stream_method_set* stream_methods = methods->stream_methods )
			{
				if ( stream_methods->write )
				{
					return stream_methods->write( &that, buffer, n );
				}
			}
			
			if ( const bstore_method_set* bstore_methods = methods->bstore_methods )
			{
				if ( bstore_methods->pwrite )
				{
					if ( that.get_flags() & O_APPEND )
					{
						return append( that, buffer, n );
					}
					
					ssize_t n_written = bstore_methods->pwrite( &that, buffer, n, that.get_mark() );
					
					return that.advance_mark( n_written );
				}
			}
		}
		
		return IOHandle_Cast< StreamHandle >( that ).Write( buffer, n );
	}
	
}


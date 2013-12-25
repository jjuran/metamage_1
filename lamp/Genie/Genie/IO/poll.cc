/*
	poll.cc
	-------
*/

#include "vfs/filehandle/primitives/poll.hh"

// vfs
#include "vfs/enum/poll_result.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/stream_method_set.hh"

// Genie
#include "Genie/IO/Stream.hh"


namespace vfs
{
	
	using namespace Genie;
	
	
	unsigned poll( filehandle& that )
	{
		if ( const filehandle_method_set* methods = that.methods() )
		{
			if ( const stream_method_set* stream_methods = methods->stream_methods )
			{
				if ( stream_methods->poll )
				{
					return stream_methods->poll( &that );
				}
				
				return Poll_read | Poll_write;
			}
			
			if ( methods->bstore_methods )
			{
				return Poll_read | Poll_write;
			}
		}
		
		return IOHandle_Cast< StreamHandle >( that ).Poll();
	}
	
}


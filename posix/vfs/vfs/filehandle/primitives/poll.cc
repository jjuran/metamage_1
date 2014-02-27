/*
	poll.cc
	-------
*/

#include "vfs/filehandle/primitives/poll.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/enum/poll_result.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/stream_method_set.hh"


namespace vfs
{
	
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
			}
		}
		
		return Poll_read | Poll_write;
	}
	
}


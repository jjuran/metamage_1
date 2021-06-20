/*
	thaw.cc
	-------
*/

#include "plus/thaw.hh"

// more-libc
#include "more/string.h"


namespace plus
{
	
	void assign_pod_from_stream( void* pod, size_t size, const char* begin, const char* end )
	{
		if ( end - begin != size )
		{
			throw thaw_size_error();
		}
		
		mempcpy( pod, begin, size );
	}
	
}

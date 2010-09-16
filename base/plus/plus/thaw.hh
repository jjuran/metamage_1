/*
	thaw.hh
	-------
*/

#ifndef PLUS_THAW_HH
#define PLUS_THAW_HH

// Standard C
#include <stddef.h>

// plus
#include "plus/string.hh"


namespace plus
{
	
	class thaw_size_error {};
	
	void assign_pod_from_stream( void* pod, size_t size, const char* begin, const char* end );
	
	template < class POD >
	struct thaw_pod
	{
		typedef POD result_type;
		
		static POD apply( const char* begin, const char* end )
		{
			POD result;
			
			assign_pod_from_stream( &result, sizeof result, begin, end );
			
			return result;
		}
	};
	
}

#endif


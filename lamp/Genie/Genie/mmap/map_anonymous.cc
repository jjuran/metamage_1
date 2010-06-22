/*
	map_anonymous.cc
	----------------
*/

#include "Genie/mmap/map_anonymous.hh"

// Standard C
#include <string.h>

// Nitrogen
#include "Nitrogen/MacMemory.hh"

// Genie
#include "Genie/mmap/Handle_memory_mapping.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	
	boost::intrusive_ptr< memory_mapping > map_anonymous( std::size_t length )
	{
		::Handle h = NULL;
		
		// dlmalloc defaults to 64K requests.  Don't use temp mem for those.
		
		if ( length < 128 * 1024 )
		{
			h = ::NewHandleClear( length );
		}
		
		n::owned< Mac::Handle > handle;
		
		if ( h != NULL )
		{
			handle = n::owned< Mac::Handle >::seize( h );
		}
		else
		{
			// Allocation above either failed or wasn't attempted.
			
			handle = N::TempNewHandle( length );
			
			memset( *handle.get(), '\0', length );
		}
		
		return seize_ptr( new Handle_memory_mapping( handle ) );
	}
	
}


/*
	map_temporary.cc
	----------------
*/

#include "vfs/mmap/functions/map_temporary.hh"

// mac-sys-utils
#include "mac_sys/has/RealTempMemory.hh"

// Nitrogen
#include "Nitrogen/MacMemory.hh"

// MacVFS
#include "MacVFS/mmap/Handle_memory_mapping.hh"


namespace vfs
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	
	static
	n::owned< Mac::Handle > new_handle( std::size_t size )
	{
		OSErr err;
		Handle h;
		
		if ( mac::sys::has_RealTempMemory() )
		{
			if (( h = TempNewHandle( size, &err ) ))  goto done;
		}
		
		if (( h = NewHandle( size ) ))  goto done;
		
		N::MemError();
		
	done:
		
		return n::owned< N::Handle >::seize( N::Handle( h ) );
	}
	
	memory_mapping_ptr map_temporary( std::size_t length, int prot, int flags )
	{
		return new Handle_memory_mapping( new_handle( length ), length, flags );
	}
	
}

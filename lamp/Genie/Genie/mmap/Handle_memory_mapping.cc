/*	========================
 *	Handle_memory_mapping.cc
 *	========================
 */

#include "Genie/mmap/Handle_memory_mapping.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	Handle_memory_mapping::Handle_memory_mapping( nucleus::owned< N::Handle > h )
	:
		its_handle( h )
	{
		N::HLock( its_handle );
	}
	
}


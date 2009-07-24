/*	========================
 *	Handle_memory_mapping.cc
 *	========================
 */

#include "Genie/mmap/Handle_memory_mapping.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	Handle_memory_mapping::Handle_memory_mapping( NN::Owned< N::Handle > h )
	:
		its_handle( h )
	{
		N::HLock( its_handle );
	}
	
}


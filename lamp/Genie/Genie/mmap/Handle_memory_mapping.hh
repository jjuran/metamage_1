/*	========================
 *	Handle_memory_mapping.hh
 *	========================
 */

#ifndef GENIE_MMAP_HANDLEMEMORYMAPPING_HH
#define GENIE_MMAP_HANDLEMEMORYMAPPING_HH

// Nitrogen
#include "Nitrogen/MacMemory.hh"

// Genie
#include "Genie/mmap/memory_mapping.hh"


namespace Genie
{
	
	class Handle_memory_mapping : public memory_mapping
	{
		private:
			nucleus::owned< Nitrogen::Handle > its_handle;
			
			addr_t get_address() const  { return *its_handle.get(); }
		
		public:
			Handle_memory_mapping( nucleus::owned< Nitrogen::Handle > h );
	};
	
}

#endif


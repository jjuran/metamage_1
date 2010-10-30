/*	========================
 *	Handle_memory_mapping.hh
 *	========================
 */

#ifndef GENIE_MMAP_HANDLEMEMORYMAPPING_HH
#define GENIE_MMAP_HANDLEMEMORYMAPPING_HH

// nucleus
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
#endif

// Nitrogen
#ifndef MAC_MEMORY_TYPES_HANDLE_HH
#include "Mac/Memory/Types/Handle.hh"
#endif

// Genie
#include "Genie/mmap/memory_mapping.hh"


namespace Genie
{
	
	class Handle_memory_mapping : public memory_mapping
	{
		private:
			nucleus::owned< Mac::Handle > its_handle;
			
			addr_t get_address() const  { return *its_handle.get(); }
		
		public:
			Handle_memory_mapping( nucleus::owned< Mac::Handle > h );
	};
	
}

#endif


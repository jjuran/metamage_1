/*	========================
 *	static_memory_mapping.hh
 *	========================
 */

#ifndef GENIE_MMAP_STATICMEMORYMAPPING_HH
#define GENIE_MMAP_STATICMEMORYMAPPING_HH

// Genie
#include "Genie/mmap/memory_mapping.hh"


namespace Genie
{
	
	class static_memory_mapping : public memory_mapping
	{
		private:
			addr_t its_address;
			
			addr_t get_address() const  { return its_address; }
		
		public:
			static_memory_mapping( addr_t addr ) : its_address( addr )
			{
			}
	};
	
}

#endif


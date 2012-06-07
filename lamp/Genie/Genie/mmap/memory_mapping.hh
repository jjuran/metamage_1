/*	=================
 *	memory_mapping.hh
 *	=================
 */

#ifndef GENIE_MMAP_MEMORYMAPPING_HH
#define GENIE_MMAP_MEMORYMAPPING_HH

// plus
#include "plus/ref_count.hh"


namespace Genie
{
	
	class memory_mapping : public plus::ref_count< memory_mapping >
	{
		public:
			typedef void* addr_t;
			
			virtual ~memory_mapping();
			
			virtual addr_t get_address() const = 0;
	};
	
}

#endif


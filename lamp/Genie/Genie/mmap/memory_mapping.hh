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
		private:
			void* const its_address;
		
		public:
			typedef void* addr_t;
			
			memory_mapping( void* addr ) : its_address( addr )
			{
			}
			
			virtual ~memory_mapping();
			
			addr_t get_address() const  { return its_address; }
	};
	
}

#endif


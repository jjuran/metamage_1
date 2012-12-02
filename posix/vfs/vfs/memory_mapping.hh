/*
	memory_mapping.hh
	-----------------
*/

#ifndef VFS_MEMORYMAPPING_HH
#define VFS_MEMORYMAPPING_HH

// plus
#include "plus/ref_count.hh"


namespace vfs
{
	
	class memory_mapping : public plus::ref_count< memory_mapping >
	{
		private:
			typedef void* addr_t;
			
			const addr_t its_address;
		
		public:
			memory_mapping( addr_t addr ) : its_address( addr )
			{
			}
			
			virtual ~memory_mapping();
			
			addr_t get_address() const  { return its_address; }
	};
	
}

#endif


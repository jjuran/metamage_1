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


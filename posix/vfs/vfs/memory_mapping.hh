/*
	memory_mapping.hh
	-----------------
*/

#ifndef VFS_MEMORYMAPPING_HH
#define VFS_MEMORYMAPPING_HH

// more-libc
#include "more/size.h"

// plus
#include "plus/ref_count.hh"


namespace vfs
{
	
	class memory_mapping : public plus::ref_count< memory_mapping >
	{
		private:
			typedef void* addr_t;
			
			const addr_t its_address;
			const size_t its_size;
			const int    its_flags;
		
		public:
			memory_mapping( addr_t addr, std::size_t size, int flags )
			:
				its_address( addr  ),
				its_size   ( size  ),
				its_flags  ( flags )
			{
			}
			
			virtual ~memory_mapping();
			
			addr_t get_address() const  { return its_address; }
			size_t get_size   () const  { return its_size;    }
			int    get_flags  () const  { return its_flags;   }
	};
	
}

#endif


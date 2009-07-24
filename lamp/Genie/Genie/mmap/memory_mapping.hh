/*	=================
 *	memory_mapping.hh
 *	=================
 */

#ifndef GENIE_MMAP_MEMORYMAPPING_HH
#define GENIE_MMAP_MEMORYMAPPING_HH

// Boost
#include <boost/shared_ptr.hpp>


namespace Genie
{
	
	class memory_mapping
	{
		public:
			typedef boost::shared_ptr< memory_mapping > shared_ptr;
			
			typedef void* addr_t;
			
			virtual ~memory_mapping();
			
			virtual addr_t get_address() const = 0;
	};
	
}

#endif


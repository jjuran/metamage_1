/*	=================
 *	memory_mapping.hh
 *	=================
 */

#ifndef GENIE_MMAP_MEMORYMAPPING_HH
#define GENIE_MMAP_MEMORYMAPPING_HH

// Debug
#include "debug/boost_assert.hh"

// Boost
#include <boost/intrusive_ptr.hpp>

// plus
#include "plus/ref_count.hh"


namespace Genie
{
	
	class memory_mapping : public plus::ref_count< memory_mapping >
	{
		public:
			typedef boost::intrusive_ptr< memory_mapping > intrusive_ptr;
			
			typedef void* addr_t;
			
			virtual ~memory_mapping();
			
			virtual addr_t get_address() const = 0;
	};
	
	
	inline memory_mapping::intrusive_ptr seize_ptr( memory_mapping* map )
	{
		return memory_mapping::intrusive_ptr( map );
	}
	
}

#endif


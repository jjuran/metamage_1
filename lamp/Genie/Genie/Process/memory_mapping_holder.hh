/*	========================
 *	memory_mapping_holder.hh
 *	========================
 */

#ifndef GENIE_PROCESS_MEMORYMAPPINGHOLDER_HH
#define GENIE_PROCESS_MEMORYMAPPINGHOLDER_HH

// Debug
#include "debug/boost_assert.hh"

// Boost
#include <boost/intrusive_ptr.hpp>

// Genie
#include "Genie/mmap/memory_mapping.hh"
#include "Genie/Utilities/simple_map.hh"


namespace Genie
{
	
	class memory_mapping;
	
	
	class memory_mapping_holder
	{
		private:
			typedef boost::intrusive_ptr< memory_mapping > intrusive_ptr;
			
			typedef void* addr_t;
			
			typedef simple_map< addr_t, intrusive_ptr > map;
			
			map its_mappings;
		
		public:
			memory_mapping_holder()
			{
			}
			
			memory_mapping_holder( const memory_mapping_holder& other );
			
			addr_t add_memory_mapping( const intrusive_ptr& mapping );
			
			void remove_memory_mapping( addr_t key );
			
			void clear_memory_mappings();
	};
	
}

#endif


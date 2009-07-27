/*	========================
 *	memory_mapping_holder.hh
 *	========================
 */

#ifndef GENIE_PROCESS_MEMORYMAPPINGHOLDER_HH
#define GENIE_PROCESS_MEMORYMAPPINGHOLDER_HH

// Standard C++
#include <map>

// Boost
#include <boost/shared_ptr.hpp>


namespace Genie
{
	
	class memory_mapping;
	
	
	class memory_mapping_holder
	{
		private:
			typedef boost::shared_ptr< memory_mapping > shared_ptr;
			
			typedef void* addr_t;
			
			typedef std::map< addr_t, shared_ptr > map;
			
			map its_mappings;
		
		public:
			memory_mapping_holder()
			{
			}
			
			memory_mapping_holder( const memory_mapping_holder& other );
			
			addr_t add_memory_mapping( const shared_ptr& mapping );
			
			void remove_memory_mapping( addr_t key );
			
			void clear_memory_mappings();
	};
	
}

#endif


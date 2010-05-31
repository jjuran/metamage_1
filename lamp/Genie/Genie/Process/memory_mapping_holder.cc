/*	========================
 *	memory_mapping_holder.cc
 *	========================
 */

#include "Genie/Process/memory_mapping_holder.hh"

// Genie
#include "Genie/mmap/memory_mapping.hh"


namespace Genie
{
	
	memory_mapping_holder::memory_mapping_holder( const memory_mapping_holder& other )
	:
		its_mappings( other.its_mappings )
	{
	}
	
	void* memory_mapping_holder::add_memory_mapping( const intrusive_ptr& mapping )
	{
		void* key = mapping->get_address();
		
		its_mappings[ key ] = mapping;
		
		return key;
	}
	
	void memory_mapping_holder::remove_memory_mapping( void* key )
	{
		its_mappings.erase( key );
	}
	
	void memory_mapping_holder::clear_memory_mappings()
	{
		its_mappings.clear();
	}
	
}


/*
	memory_tract.cc
	---------------
*/

#include "relix/task/memory_tract.hh"

// POSIX
#include <sys/mman.h>


#pragma exceptions off


namespace relix
{
	
	typedef void* addr_t;
	
	
	allocation_result memory_tract::allocate()
	{
		if ( its_mapping->get_flags() & MAP_PRIVATE )
		{
			const size_t size = its_mapping->get_size();
			
			return its_swap.allocate( size );
		}
		
		return Allocation_unnecessary;
	}
	
	void memory_tract::back_up()
	{
		if ( its_swap.allocated() )
		{
			const addr_t addr = its_mapping->get_address();
			const size_t size = its_mapping->get_size   ();
			
			its_swap.back_up( addr, size );
		}
	}
	
	void memory_tract::restore()
	{
		if ( its_swap.allocated() )
		{
			const addr_t addr = its_mapping->get_address();
			const size_t size = its_mapping->get_size   ();
			
			its_swap.restore( addr, size );
			
			if ( its_mapping->ref_count() == 1 )
			{
				its_swap.reset();
			}
		}
	}
	
}

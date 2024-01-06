/*
	resource_handle.hh
	------------------
*/

#ifndef GENIE_CODE_RESOURCEHANDLE_HH
#define GENIE_CODE_RESOURCEHANDLE_HH

// Genie
#include "Genie/code/loaded_handle.hh"


namespace Genie
{
	
	extern unsigned long address_mask;
	
	inline
	void* masked( void* address )
	{
	#ifdef __MC68K__
		
		address = (void*) ((unsigned long) address & address_mask);
		
	#endif
		
		return address;
	}
	
	class resource_handle : public loaded_handle
	{
		public:
			resource_handle( const execution_unit& exec )
			:
				loaded_handle( exec )
			{
				its_relix_main = (vfs::relix_entry) masked( *unit().get() );
			}
	};
	
#ifndef __POWERPC__
	
	typedef resource_handle native_code_handle;
	
#endif
	
}

#endif

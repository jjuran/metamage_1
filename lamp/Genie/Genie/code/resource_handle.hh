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
	
	class resource_handle : public loaded_handle
	{
		public:
			resource_handle( const execution_unit& exec )
			:
				loaded_handle( exec )
			{
				its_relix_main = (vfs::relix_entry) *unit().get();
			}
	};
	
#ifndef __POWERPC__
	
	typedef resource_handle native_code_handle;
	
#endif
	
}

#endif

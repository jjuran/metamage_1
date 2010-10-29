/*
	fragment_handle.hh
	------------------
*/

#ifndef GENIE_CODE_FRAGMENTHANDLE_HH
#define GENIE_CODE_FRAGMENTHANDLE_HH

// nucleus
#include "nucleus/owned.hh"

// Nitrogen
#include "Mac/CodeFragments/Types/CFragConnectionID.hh"

// Genie
#include "Genie/code/loaded_handle.hh"


namespace Genie
{
	
	class fragment_handle : public loaded_handle
	{
		private:
			lamp_entry its_lamp_main;
			
			nucleus::owned< CFragConnectionID >  its_fragment_connection;
		
		public:
			fragment_handle( const execution_unit& exec );
			
			~fragment_handle();
			
			lamp_entry get_main_entry_point() const;
	};
	
#ifdef __POWERPC__
	
	typedef fragment_handle native_code_handle;
	
#endif
	
}

#endif


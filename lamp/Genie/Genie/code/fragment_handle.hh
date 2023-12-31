/*
	fragment_handle.hh
	------------------
*/

#ifndef GENIE_CODE_FRAGMENTHANDLE_HH
#define GENIE_CODE_FRAGMENTHANDLE_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __CODEFRAGMENTS__
#include <CodeFragments.h>
#endif

// Genie
#include "Genie/code/loaded_handle.hh"


namespace Genie
{
	
	class fragment_handle : public loaded_handle
	{
		private:
			CFragConnectionID  its_fragment_connection;
			
			// non-copyable
			fragment_handle           ( const fragment_handle& );
			fragment_handle& operator=( const fragment_handle& );
		
		public:
			fragment_handle( const execution_unit& exec );
			
			~fragment_handle();
			
			vfs::relix_entry get_main_entry_point() const;
	};
	
#ifdef __POWERPC__
	
	typedef fragment_handle native_code_handle;
	
#endif
	
}

#endif

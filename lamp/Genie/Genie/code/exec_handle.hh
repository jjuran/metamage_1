/*
	exec_handle.hh
	--------------
*/

#ifndef GENIE_CODE_EXECHANDLE_HH
#define GENIE_CODE_EXECHANDLE_HH

// plus
#include "plus/ref_count.hh"


struct _relix_system_parameter_block;


namespace vfs
{
	
	typedef int (*relix_entry)( int                             argc,
	                            char**                          argv,
	                            char**                          envp,
	                            _relix_system_parameter_block*  pb );
	
	class program : public plus::ref_count< program >
	{
		public:
			virtual ~program();
			
			virtual relix_entry get_main_entry_point() const = 0;
	};
	
}

namespace Genie
{
	
	using vfs::relix_entry;
	
	typedef vfs::program exec_handle;
	
}

#endif


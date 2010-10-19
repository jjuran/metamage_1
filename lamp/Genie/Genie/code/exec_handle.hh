/*
	exec_handle.hh
	--------------
*/

#ifndef GENIE_CODE_EXECHANDLE_HH
#define GENIE_CODE_EXECHANDLE_HH

// plus
#include "plus/ref_count.hh"


struct _lamp_system_parameter_block;


namespace Genie
{
	
	typedef int (*lamp_entry)( int                            argc,
	                           char**                         argv,
	                           char**                         envp,
	                           _lamp_system_parameter_block*  pb );
	
	class exec_handle : public plus::ref_count< exec_handle >
	{
		public:
			virtual ~exec_handle();
			
			virtual lamp_entry get_main_entry_point() const = 0;
	};
	
}

#endif


/*
	program.hh
	----------
*/

#ifndef VFS_PROGRAM_HH
#define VFS_PROGRAM_HH

// vxo
#include "vxo/ref_count.hh"


struct _relix_system_parameter_block;


namespace vfs
{
	
	typedef int (*relix_entry)( int                             argc,
	                            char* const*                    argv,
	                            char* const*                    envp,
	                            _relix_system_parameter_block*  pb );
	
	class program : public vxo::ref_count< program >
	{
		public:
			virtual ~program();
			
			virtual relix_entry get_main_entry_point() const = 0;
	};
	
}

#endif

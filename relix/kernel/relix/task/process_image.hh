/*
	process_image.hh
	----------------
*/

#ifndef RELIX_TASK_PROCESSIMAGE_HH
#define RELIX_TASK_PROCESSIMAGE_HH

// relix-include
#include "relix/parameter_block.h"

// plus
#include "plus/ref_count.hh"

// vfs
#include "vfs/node_ptr.hh"
#include "vfs/program_ptr.hh"


namespace relix
{
	
	class process_image : public plus::ref_count< process_image >
	{
		private:
			typedef _relix_user_parameter_block pb;
			
			_relix_user_parameter_block its_pb;
			
			vfs::node_ptr its_exe;
			
			vfs::program_ptr its_program;
		
		public:
			process_image();
			
			process_image( const vfs::node& exe );
			
			~process_image();
			
			pb& initialize_param_block( const void* bottom, const void* limit );
			
			int get_errno() const;
			
			int set_errno( int errnum ) const;
			
			const vfs::node& get_exe() const;
			
			int enter_start_routine( int                             argc,
			                         char* const*                    argv,
			                         char* const*                    envp,
			                         _relix_system_parameter_block*  pb );
	};
	
}

#endif


/*
	process_image.cc
	----------------
*/

#include "relix/task/process_image.hh"

// relix
#include "relix/glue/userland.hh"


namespace relix
{
	
	typedef _relix_user_parameter_block pb;
	
	
	process_image::process_image()
	{
		const _relix_user_parameter_block init = { 0 };  // NULL
		
		its_pb = init;
	}
	
	process_image::~process_image()
	{
		if ( its_pb.cleanup != 0 )  // NULL
		{
			ENTER_USERLAND( its_pb.globals );
			
			its_pb.cleanup();
			
			EXIT_USERLAND();
		}
	}
	
	pb& process_image::initialize_param_block( const void* bottom, const void* limit )
	{
		its_pb.stack_bottom = bottom;
		its_pb.stack_limit  = limit;
		
		return its_pb;
	}
	
	int process_image::get_errno() const
	{
		if ( its_pb.errno_var != 0 )  // NULL
		{
			return *its_pb.errno_var;
		}
		
		return 0;
	}
	
	int process_image::set_errno( int errnum ) const
	{
		if ( its_pb.errno_var != 0 )  // NULL
		{
			*its_pb.errno_var = errnum;
		}
		
		return errnum == 0 ? 0 : -1;
	}
	
}


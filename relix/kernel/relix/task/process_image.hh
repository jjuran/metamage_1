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


namespace relix
{
	
	class process_image : public plus::ref_count< process_image >
	{
		private:
			typedef _relix_user_parameter_block pb;
			
			_relix_user_parameter_block its_pb;
		
		public:
			process_image();
			
			~process_image();
			
			pb& initialize_param_block( const void* bottom, const void* limit );
			
			int get_errno() const;
			
			int set_errno( int errnum ) const;
	};
	
}

#endif


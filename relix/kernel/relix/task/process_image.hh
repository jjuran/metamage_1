/*
	process_image.hh
	----------------
*/

#ifndef RELIX_TASK_PROCESSIMAGE_HH
#define RELIX_TASK_PROCESSIMAGE_HH

// relix-include
#include "relix/parameter_block.h"

// Debug
#include "debug/boost_assert.hh"

// Boost
#include <boost/intrusive_ptr.hpp>

// vxo
#include "vxo/ref_count.hh"

// vfs
#include "vfs/memory_mapping_fwd.hh"
#include "vfs/node_ptr.hh"
#include "vfs/program_ptr.hh"

// relix-kernel
#include "relix/task/memory_data.hh"


namespace relix
{
	
	class process_image : public vxo::ref_count< process_image >
	{
		private:
			typedef _relix_user_parameter_block pb;
			
			_relix_user_parameter_block its_pb;
			
			vfs::node_ptr its_exe;
			
			vfs::program_ptr its_program;
			
			boost::intrusive_ptr< memory_data > its_memory_data;
			
			process_image( const process_image& other );  // privately copyable
		
		public:
			process_image();
			
			process_image( const vfs::node&    exe,
			               const char* const*  argv,
			               const char* const*  envp );
			
			~process_image();
			
			process_image* clone() const  { return new process_image( *this ); }
			
			pb& param_block()  { return its_pb; }
			
			int get_errno() const;
			
			int set_errno( int errnum ) const;
			
			const vfs::node& get_exe() const;
			
			int enter_start_routine( _relix_system_parameter_block* pb );
			
			const plus::string& get_cmdline() const;
			
			typedef void* addr_t;
			
			addr_t add_memory_mapping( const vfs::memory_mapping* mapping );
			
			void msync_memory_mapping( addr_t addr, size_t len, int flags );
			
			void remove_memory_mapping( addr_t key );
			
			void back_up_memory();
			void restore_memory();
			
			void switch_in();
			void switch_out();
	};
	
}

#endif

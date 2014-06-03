/*
	process_image.cc
	----------------
*/

#include "relix/task/process_image.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/node.hh"

// vfs-relix
#include "vfs/program.hh"
#include "vfs/primitives/exec.hh"

// relix
#include "relix/glue/userland.hh"
#include "relix/task/A5_world.hh"


#if defined( __MC68K__ )  &&  defined( __MACOS__ )
#define RELIX_A5_WORLD
#endif


namespace relix
{
	
	namespace p7 = poseven;
	
	
	typedef _relix_user_parameter_block pb;
	
	
	static inline boost::intrusive_ptr< memory_data > root_memory_data()
	{
		boost::intrusive_ptr< memory_data > result( memory_data::create() );
		
		char const *const argv[] = { "init", NULL };
		
		result->set_argv( argv );
		result->set_envp( NULL );
		
		return result;
	}
	
	process_image::process_image() : its_memory_data( root_memory_data() )
	{
		const _relix_user_parameter_block init = { 0 };  // NULL
		
		its_pb = init;
	}
	
	process_image::process_image( const vfs::node&    exe,
	                              const char* const*  argv,
	                              const char* const*  envp )
	:
		its_exe( &exe ),
		its_program( exec( exe ) ),
		its_memory_data( memory_data::create() )
	{
		const _relix_user_parameter_block init = { 0 };  // NULL
		
		its_pb = init;
		
	#ifdef RELIX_A5_WORLD
		
		its_pb.a5_world = the_saved_A5_world;  // copy the host's A5 world
		
	#endif
		
		its_memory_data->set_argv( argv );
		its_memory_data->set_envp( envp );
	}
	
	process_image::process_image( const process_image& other )
	:
		its_pb( other.its_pb ),
		its_exe( other.its_exe ),
		its_program( other.its_program ),
		its_memory_data( other.its_memory_data->clone() )
	{
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
	
	const vfs::node& process_image::get_exe() const
	{
		if ( its_exe.get() == 0 )  // NULL
		{
			p7::throw_errno( ENOENT );
		}
		
		return *its_exe;
	}
	
	int process_image::enter_start_routine( _relix_system_parameter_block* pb )
	{
		int          argc = its_memory_data->get_argc();
		char* const* argv = its_memory_data->get_argv();
		char* const* envp = its_memory_data->get_envp();
		
		vfs::relix_entry relix_main = its_program->get_main_entry_point();
		
		ENTER_USERMAIN();
		
		int exit_status = relix_main( argc, argv, envp, pb );
		
		EXIT_USERMAIN();
		
		// Not reached by regular tools, since they call exit()
		
		return exit_status;
	}
	
	const plus::string& process_image::get_cmdline() const
	{
		return its_memory_data.get() ? its_memory_data.get()->get_cmdline()
		                             : plus::string::null;
	}
	
	void* process_image::add_memory_mapping( const vfs::memory_mapping* mapping )
	{
		return its_memory_data->add_memory_mapping( mapping );
	}
	
	void process_image::msync_memory_mapping( addr_t addr, size_t len, int flags )
	{
		its_memory_data->msync_memory_mapping( addr, len, flags );
	}
	
	void process_image::remove_memory_mapping( addr_t key )
	{
		its_memory_data->remove_memory_mapping( key );
	}
	
	void process_image::back_up_memory()
	{
		if ( memory_data* memory = its_memory_data.get() )
		{
			memory->back_up();
		}
	}
	
	void process_image::restore_memory()
	{
		if ( memory_data* memory = its_memory_data.get() )
		{
			memory->restore();
		}
	}
	
	void process_image::switch_in()
	{
	#ifdef RELIX_A5_WORLD
		
		load_A5_world( (a5_world_t) its_pb.a5_world );
		
	#endif
	}
	
	void process_image::switch_out()
	{
	#ifdef RELIX_A5_WORLD
		
		its_pb.a5_world = swap_A5_worlds();
		
	#endif
	}
	
}

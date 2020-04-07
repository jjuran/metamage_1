/*	==============
 *	FSTree_Proc.cc
 *	==============
 */

#include "Genie/FS/FSTree_Proc.hh"

// POSIX
#include <fcntl.h>
#include <sys/stat.h>

// Standard C
#include <time.h>

// gear
#include "gear/inscribe_decimal.hh"
#include "gear/parse_decimal.hh"

// plus
#include "plus/contains.hh"
#include "plus/var_string.hh"

// Recall
#include "recall/backtrace.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/dir_contents.hh"
#include "vfs/dir_entry.hh"
#include "vfs/filehandle.hh"
#include "vfs/file_descriptor.hh"
#include "vfs/node.hh"
#include "vfs/property.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/primitives/geteof.hh"
#include "vfs/filehandle/primitives/getpgrp.hh"
#include "vfs/filehandle/primitives/get_file.hh"
#include "vfs/filehandle/types/shadow.hh"
#include "vfs/functions/pathname.hh"
#include "vfs/methods/data_method_set.hh"
#include "vfs/methods/dir_method_set.hh"
#include "vfs/methods/item_method_set.hh"
#include "vfs/methods/link_method_set.hh"
#include "vfs/methods/node_method_set.hh"
#include "vfs/node/types/basic_directory.hh"
#include "vfs/node/types/fixed_dir.hh"
#include "vfs/node/types/generated_file.hh"
#include "vfs/node/types/property_file.hh"

// relix-kernel
#include "relix/api/current_process.hh"
#include "relix/api/getcwd.hh"
#include "relix/api/getexe.hh"
#include "relix/api/root.hh"
#include "relix/task/fd_map.hh"
#include "relix/task/process.hh"
#include "relix/task/process_group.hh"
#include "relix/task/process_image.hh"
#include "relix/task/session.hh"

// Genie
#include "Genie/Process.hh"
#include "Genie/ProcessList.hh"
#include "Genie/Utilities/canonical_positive_integer.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	static pid_t GetKeyFromParent( const vfs::node* parent )
	{
		return pid_t( gear::parse_unsigned_decimal( parent->name().c_str() ) );
	}
	
	
	static const relix::fd_map& fd_sequence( const vfs::node* that )
	{
		const pid_t pid = gear::parse_unsigned_decimal( that->owner()->name().c_str() );
		
		return GetProcess( pid ).FileDescriptors();
	}
	
	static void iterate_one_fd( void* param, int fd, const vfs::file_descriptor& )
	{
		const ino_t inode = fd;
		
		plus::string name = gear::inscribe_decimal( fd );
		
		vfs::dir_contents& cache = *(vfs::dir_contents*) param;
		
		cache.push_back( vfs::dir_entry( inode, name ) );
	}
	
	static vfs::node_ptr proc_fd_lookup( const vfs::node*     that,
	                                     const plus::string&  name,
	                                     const vfs::node*     parent );
	
	static void proc_fd_listdir( const vfs::node* that, vfs::dir_contents& cache )
	{
		const relix::fd_map& sequence = fd_sequence( that );
		
		sequence.for_each( &iterate_one_fd, &cache );
	}
	
	static const vfs::dir_method_set proc_fd_dir_methods =
	{
		&proc_fd_lookup,
		&proc_fd_listdir
	};
	
	static const vfs::node_method_set proc_fd_methods =
	{
		NULL,
		NULL,
		NULL,
		&proc_fd_dir_methods
	};
	
	
	static vfs::filehandle_ptr proc_fd_link_open( const vfs::node* that, int flags, mode_t mode );
	
	static off_t proc_fd_link_geteof( const vfs::node* that );
	
	static vfs::node_ptr proc_fd_link_resolve( const vfs::node* that );
	
	static const vfs::data_method_set proc_fd_link_data_methods =
	{
		&proc_fd_link_open,
		&proc_fd_link_geteof
	};
	
	static const vfs::link_method_set proc_fd_link_link_methods =
	{
		NULL,
		&proc_fd_link_resolve
	};
	
	static const vfs::node_method_set proc_fd_link_methods =
	{
		NULL,
		&proc_fd_link_data_methods,
		&proc_fd_link_link_methods
	};
	
	
	static vfs::node_ptr proc_link_resolve( const vfs::node* that )
	{
		const pid_t pid = GetKeyFromParent( that->owner() );
		
		Process& process = GetProcess( pid );
		
		const char* name = that->name().c_str();
		
		switch ( name[0] )
		{
			case 'c':  // cwd
				return getcwd( process.get_process() );
			
			case 'e':  // exe
				return getexe( process.get_process() );
			
			case 'r':  // root
				return relix::root();
			
			default:
				// not reached
				return NULL;
		}
	}
	
	static const vfs::link_method_set proc_link_link_methods =
	{
		NULL,
		&proc_link_resolve
	};
	
	static const vfs::node_method_set proc_link_methods =
	{
		NULL,
		NULL,
		&proc_link_link_methods
	};
	
	
	extern const vfs::fixed_mapping proc_PID_Mappings[];
	
	struct valid_name_of_pid
	{
		typedef canonical_positive_integer well_formed_name;
		
		static bool applies( const plus::string& name )
		{
			return    well_formed_name::applies( name )
			       && process_exists( gear::parse_unsigned_decimal( name.c_str() ) );
		}
	};
	
	static plus::string proc_self_readlink( const vfs::node* that )
	{
		return gear::inscribe_unsigned_decimal( CurrentProcess().GetPID() );
	}
	
	static const vfs::link_method_set proc_self_link_methods =
	{
		&proc_self_readlink
	};
	
	static const vfs::node_method_set proc_self_methods =
	{
		NULL,
		NULL,
		&proc_self_link_methods
	};
	
	
	static vfs::node_ptr proc_lookup( const vfs::node* parent, const plus::string& name )
	{
		if ( name == "self" )
		{
			return new vfs::node( parent, name, S_IFLNK | 0777, &proc_self_methods );
		}
		
		if ( !valid_name_of_pid::applies( name ) )
		{
			p7::throw_errno( ENOENT );
		}
		
		return fixed_dir( parent, name, proc_PID_Mappings );
	}
	
	static void* iterate_one_process( void* param, pid_t pid, Process& )
	{
		const ino_t inode = pid;
		
		plus::string name = gear::inscribe_decimal( pid );
		
		vfs::dir_contents& cache = *(vfs::dir_contents*) param;
		
		cache.push_back( vfs::dir_entry( inode, name ) );
		
		return NULL;
	}
	
	static void proc_iterate( const vfs::node* parent, vfs::dir_contents& cache )
	{
		for_each_process( &iterate_one_process, &cache );
	}
	
	// Process states
	// --------------
	
	/*
		pid 1:  stateless, parentless
		
		other:
			Lifetime:
				starting --> live --> terminated --> released
				
				NEW --[spawned]--> starting
				
				starting --[thread entry]--> live
				starting --[fatal signal]--> terminated
				
				live --[exited]--> terminated
				live --[signal]--> terminated
				
				terminated --[wait]--> released
				
				released --[reaped]--> DESTROYED
				
			Interdependence:
				independent --> forking/forked --> independent/independent
			
			Schedule:
				NEW --[vfork]--> running
				
				running --[yield  ]--> sleeping
				running --[exec   ]--> sleeping
				running --[SIGSTOP]--> stopped
				running --[vfork  ]--> frozen
				
				sleeping --[unyield]--> running
				sleeping --[SIGSTOP]--> stopped
				
				stopped --[SIGCONT]--> sleeping
				stopped --[SIGCONT]--> running
				
				frozen --[execve ]--> running
				frozen --[_exit  ]--> running
				
			Notes:
				A process begins as starting and forked, remaining so until it
				exec's, at which point it's live and independent (and sleeping).
				
				Forking processes are frozen until the child exec's.
				
				Forks may not be nested; therefore starting/forked processes may not be frozen.
				
				Terminated and released processes are not scheduled.
				
				A forked thread may call _exit, which effectively undoes the fork.
				The parent is then once again independent and running.
				
				Signals sent to a forking process are blocked while it's frozen.
				Fatal signals sent to a forked process...
				
	*/
	
	static char ProcessStateCode( const Process& process )
	{
		char runState = process.run_state_code();
		
		if ( runState == 'S'  &&  clock() - process.get_process().get_last_activity() > 20 * 1000 * 1000 )
		{
			runState = 'I';  // idle
		}
		
		return runState;
	}
	
	class proc_PID_cmdline
	{
		public:
			static plus::string Get( const Process& process )
			{
				return process.get_process().get_cmdline();
			}
	};
	
	class proc_PID_stat
	{
		public:
			static plus::string Get( const Process& process )
			{
				// `process` is actually a thread
				
				relix::process&        proc    = process.get_process();
				relix::process_group&  group   = proc.get_process_group();
				relix::session&        session = group.get_session();
				
				pid_t pid  = process.GetPID();
				pid_t ppid = proc.getppid();
				pid_t pgid = group.id();
				pid_t sid  = session.id();
				
				vfs::filehandle* term = session.get_ctty().get();
				
				plus::string terminal_name = "?";
				
				pid_t tpgid = 0;
				
				if ( term != NULL )
				{
					terminal_name = pathname( *get_file( *term ) );
					
					tpgid = getpgrp( *term );
				}
				
				plus::var_string result;
				
				result += gear::inscribe_decimal( pid );
				result += " "
				          "(";
				result += proc.name();
				result += ")"
				          " ";
				result += ProcessStateCode( process );
				result += " ";
				result += gear::inscribe_decimal( ppid );
				result += " ";
				result += gear::inscribe_decimal( pgid );
				result += " ";
				result += gear::inscribe_decimal( sid );
				result += " ";
				result += terminal_name;
				result += " ";
				result += gear::inscribe_decimal( tpgid );
				result += "\n";
				
				return result;
			}
	};
	
	class proc_PID_stack
	{
		public:
			static plus::string Get( const Process& process );
	};
	
	plus::string proc_PID_stack::Get( const Process& process )
	{
		using recall::stack_frame_pointer;
		using recall::frame_data;
		
		stack_frame_pointer top = process.marked_stack_frame();
		
		if ( top == NULL )
		{
			return plus::string::null;
		}
		
		const unsigned frame_capacity = 64;
		
		frame_data stack_crawl[ frame_capacity ];
		
		const unsigned n_frames = make_stack_crawl( stack_crawl, frame_capacity, top );
		
		const frame_data* begin = stack_crawl;
		const frame_data* end   = stack_crawl + n_frames;
		
		// skip _relix_main
		// skip process_image::enter_start_routine()
		// skip Process::thread_start()
		// skip Process_ThreadEntry()
		// skip ??? (Thread Manager)
		// skip ??? (Thread Manager)
		
		const int n_skipped = 6;
		
		if ( end - n_skipped > begin )
		{
			end -= n_skipped;
		}
		
		plus::var_string result;
		
		make_report_from_stack_crawl( result, begin, end );
		
		return result;
	}
	
	
	template < class Accessor >
	struct proc_PID_Property
	{
		typedef pid_t Key;
		
		static plus::string Read( const vfs::node* parent, const plus::string& name )
		{
			Key pid = GetKeyFromParent( parent );
			
			return Accessor::Get( GetProcess( pid ) );
		}
	};
	
	struct proc_PID_name
	{
		static void get( plus::var_string& result, const vfs::node* that, bool binary )
		{
			pid_t pid = GetKeyFromParent( that );
			
			result = GetProcess( pid ).get_process().name();
		}
	};
	
	static const vfs::property_params proc_PID_name_params =
	{
		vfs::no_fixed_size,
		&proc_PID_name::get,
	};
	
	static vfs::node_ptr fd_Factory( const vfs::node*     parent,
	                                 const plus::string&  name,
	                                 const void*          args )
	{
		return new vfs::node( parent, name, S_IFDIR | 0700, &proc_fd_methods );
	}
	
	static vfs::node_ptr link_factory( const vfs::node*     parent,
	                                   const plus::string&  name,
	                                   const void*          args )
	{
		return new vfs::node( parent, name, S_IFLNK | 0777, &proc_link_methods );
	}
	
	static void proc_pid_core_chmod( const vfs::node*  that,
	                                 mode_t            mode )
	{
		const pid_t pid = GetKeyFromParent( that->owner() );
		
		Process& process = GetProcess( pid );
		
		if ( mode == 0 )
		{
			process.SuppressCoreDump();
		}
		else
		{
			process.AllowCoreDump();
		}
	}
	
	static const vfs::item_method_set proc_pid_core_item_methods =
	{
		NULL,
		&proc_pid_core_chmod
	};
	
	static const vfs::node_method_set proc_pid_core_methods =
	{
		&proc_pid_core_item_methods,
	};
	
	static vfs::node_ptr core_Factory( const vfs::node*     parent,
	                                   const plus::string&  name,
	                                   const void*          args )
	{
		return new vfs::node( parent, name, S_IFREG | 0600, &proc_pid_core_methods );
	}
	
	#define PROPERTY( prop )  &vfs::new_property, &prop##_params
	
	#define GENERATED( gen )  &vfs::new_generated, (void*) &proc_PID_Property< gen >::Read
	
	const vfs::fixed_mapping proc_PID_Mappings[] =
	{
		{ "fd", &fd_Factory },
		
		{ "cwd",  &link_factory },
		{ "exe",  &link_factory },
		{ "root", &link_factory },
		
		{ "name", PROPERTY( proc_PID_name ) },
		
		{ ".~name", PROPERTY( proc_PID_name ) },
		
		{ "cmdline", GENERATED( proc_PID_cmdline ) },
		{ "stat",    GENERATED( proc_PID_stat    ) },
		{ "stack",   GENERATED( proc_PID_stack   ) },
		
		{ "core", &core_Factory },
		
		{ NULL, NULL }
	};
	
	static vfs::node_ptr proc_fd_lookup( const vfs::node*     that,
	                                     const plus::string&  name,
	                                     const vfs::node*     parent )
	{
		const int key = gear::parse_unsigned_decimal( name.c_str() );
		
		const relix::fd_map& sequence = fd_sequence( that );
		
		if ( !sequence.contains( key ) )
		{
			poseven::throw_errno( ENOENT );
		}
		
		return new vfs::node( parent,
		                      name,
		                      S_IFLNK | 0777,
		                      &proc_fd_link_methods );
	}
	
	
	static vfs::filehandle* get_proc_fd_handle( const vfs::node* that )
	{
		const char* fd_name  = that                  ->name().c_str();
		const char* pid_name = that->owner()->owner()->name().c_str();
		
		const int    fd  = gear::parse_unsigned_decimal( fd_name  );
		const pid_t  pid = gear::parse_unsigned_decimal( pid_name );
		
		relix::fd_map& files = GetProcess( pid ).FileDescriptors();
		
		if ( !files.contains( fd ) )
		{
			p7::throw_errno( ENOENT );
		}
		
		return files.at( fd ).handle.get();
	}
	
	static off_t proc_fd_link_geteof( const vfs::node* that )
	{
		vfs::filehandle* handle = get_proc_fd_handle( that );
		
		if ( handle->methods()  &&  handle->methods()->bstore_methods )
		{
			return geteof( *handle );
		}
		
		return 0;
	}
	
	
	static vfs::filehandle_ptr proc_fd_link_open( const vfs::node* that, int flags, mode_t mode )
	{
		if ( flags & O_NOFOLLOW )
		{
			p7::throw_errno( ELOOP );
		}
		
		vfs::filehandle_ptr fh = get_proc_fd_handle( that );
		
		if ( fh->methods()  &&  fh->methods()->bstore_methods )
		{
			return vfs::new_shadow( *that, flags, *fh );
		}
		
		return fh;
	}
	
	static vfs::node_ptr proc_fd_link_resolve( const vfs::node* that )
	{
		return get_file( *get_proc_fd_handle( that ) );
	}
	
	vfs::node_ptr New_FSTree_proc( const vfs::node*     parent,
	                               const plus::string&  name,
	                               const void*          args )
	{
		return vfs::new_basic_directory( parent, name, proc_lookup, proc_iterate );
	}
	
}

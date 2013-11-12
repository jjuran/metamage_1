/*	==============
 *	FSTree_Proc.cc
 *	==============
 */

#include "Genie/FS/FSTree_Proc.hh"

// POSIX
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
#include "vfs/file_descriptor.hh"
#include "vfs/filehandle/methods/bstore_method_set.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/primitives/append.hh"
#include "vfs/filehandle/primitives/geteof.hh"
#include "vfs/filehandle/primitives/getpgrp.hh"
#include "vfs/filehandle/primitives/pread.hh"
#include "vfs/filehandle/primitives/pwrite.hh"
#include "vfs/filehandle/primitives/seteof.hh"
#include "vfs/functions/pathname.hh"
#include "vfs/functions/root.hh"
#include "vfs/node/types/fixed_dir.hh"

// relix-kernel
#include "relix/api/current_process.hh"
#include "relix/task/fd_table.hh"
#include "relix/task/process.hh"
#include "relix/task/process_group.hh"
#include "relix/task/session.hh"

// Genie
#include "Genie/FS/basic_directory.hh"
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/FSTree_Generated.hh"
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/data_method_set.hh"
#include "Genie/FS/dir_method_set.hh"
#include "Genie/FS/link_method_set.hh"
#include "Genie/FS/node_method_set.hh"
#include "Genie/FS/property.hh"
#include "Genie/IO/Base.hh"
#include "Genie/IO/RegularFile.hh"
#include "Genie/ProcessList.hh"
#include "Genie/Utilities/canonical_positive_integer.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	static pid_t GetKeyFromParent( const FSTree* parent )
	{
		return pid_t( gear::parse_unsigned_decimal( parent->name().c_str() ) );
	}
	
	static inline pid_t GetKeyFromParent( const FSTreePtr& parent )
	{
		return GetKeyFromParent( parent.get() );
	}
	
	
	static const relix::fd_table& fd_sequence( const FSTree* that )
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
	
	static FSTreePtr proc_fd_lookup( const FSTree*        that,
	                                 const plus::string&  name,
	                                 const FSTree*        parent );
	
	static void proc_fd_listdir( const FSTree* that, vfs::dir_contents& cache )
	{
		const relix::fd_table& sequence = fd_sequence( that );
		
		sequence.for_each( &iterate_one_fd, &cache );
	}
	
	static const dir_method_set proc_fd_dir_methods =
	{
		&proc_fd_lookup,
		&proc_fd_listdir
	};
	
	static const node_method_set proc_fd_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&proc_fd_dir_methods
	};
	
	
	static IOPtr proc_fd_link_open( const FSTree* that, int flags, mode_t mode );
	
	static off_t proc_fd_link_geteof( const FSTree* that );
	
	static FSTreePtr proc_fd_link_resolve( const FSTree* that );
	
	static const data_method_set proc_fd_link_data_methods =
	{
		&proc_fd_link_open,
		&proc_fd_link_geteof
	};
	
	static const link_method_set proc_fd_link_link_methods =
	{
		NULL,
		&proc_fd_link_resolve
	};
	
	static const node_method_set proc_fd_link_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&proc_fd_link_data_methods,
		&proc_fd_link_link_methods
	};
	
	
	static FSTreePtr proc_link_resolve( const FSTree* that )
	{
		const pid_t pid = GetKeyFromParent( that->owner() );
		
		Process& process = GetProcess( pid );
		
		const char* name = that->name().c_str();
		
		switch ( name[0] )
		{
			case 'c':  // cwd
				return process.GetCWD();
			
			case 'e':  // exe
				return process.ProgramFile();
			
			case 'r':  // root
				return vfs::root();
			
			default:
				// not reached
				return NULL;
		}
	}
	
	static const link_method_set proc_link_link_methods =
	{
		NULL,
		&proc_link_resolve
	};
	
	static const node_method_set proc_link_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
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
	
	static plus::string proc_self_readlink( const FSTree* that )
	{
		return gear::inscribe_unsigned_decimal( CurrentProcess().GetPID() );
	}
	
	static const link_method_set proc_self_link_methods =
	{
		&proc_self_readlink
	};
	
	static const node_method_set proc_self_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&proc_self_link_methods
	};
	
	
	static FSTreePtr proc_lookup( const FSTree* parent, const plus::string& name )
	{
		if ( name == "self" )
		{
			return new FSTree( parent, name, S_IFLNK | 0777, &proc_self_methods );
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
	
	static void proc_iterate( const FSTree* parent, vfs::dir_contents& cache )
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
	
	static char ProcessRunStateCode( ProcessSchedule schedule )
	{
		switch ( schedule )
		{
			case kProcessRunning:      return 'R';  // [1]
			case kProcessSleeping:     return 'S';  // [2]
			case kProcessStopped:      return 'T';  // set in Process::Stop()
			case kProcessFrozen:       return 'V';  // set in SpawnVFork() prior to NewProcess()
			case kProcessUnscheduled:  return 'Z';  // set in Process::Terminate()
			
			// [1] set on parent in execve() after child.Exec()
			//     set on parent in _exit() if forked
			//     set in Yield() after YieldToAnyThread() returns
			//     set in StopThread() after SetThreadState() if same thread
			// [2] set at end of Process::Exec() (after creating new thread for child)
			//     set in Process::Continue() if thread was stopped
			//     set in Yield() before YieldToAnyThread() is called
			
			default:
				return '?';
		}
	}
	
	static char ProcessStateCode( const Process& process )
	{
		ProcessSchedule schedule = process.GetSchedule();
		
		ProcessLifeStage lifestage = process.GetLifeStage();
		
		char runState = ProcessRunStateCode( schedule );
		
		if ( lifestage == kProcessReleased )
		{
			runState = 'X';
		}
		else if ( process.CountAsyncOps() > 0 )
		{
			runState = 'D';
		}
		else if ( runState == 'S'  &&  clock() - relix::current_process().get_last_activity() > 20 * 1000 * 1000 )
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
				return process.GetCmdLine();
			}
	};
	
	class proc_PID_stat
	{
		public:
			static plus::string Get( const Process& process )
			{
				pid_t pid  = process.GetPID();
				pid_t ppid = process.GetPPID();
				pid_t pgid = process.GetPGID();
				pid_t sid  = process.GetSID();
				
				vfs::filehandle* term = process.get_process().get_process_group().get_session().get_ctty().get();
				
				plus::string terminal_name = "?";
				
				pid_t tpgid = 0;
				
				if ( term != NULL )
				{
					terminal_name = pathname( *term->GetFile() );
					
					tpgid = getpgrp( *term );
				}
				
				plus::var_string result;
				
				result += gear::inscribe_decimal( pid );
				result += " "
				          "(";
				result += process.ProgramName();
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
		
		stack_frame_pointer top = process.GetStackFramePointer();
		
		if ( top == NULL )
		{
			return plus::string::null;
		}
		
		const unsigned frame_capacity = 64;
		
		frame_data stack_crawl[ frame_capacity ];
		
		const unsigned n_frames = make_stack_crawl( stack_crawl, frame_capacity, top );
		
		const frame_data* begin = stack_crawl;
		const frame_data* end   = stack_crawl + n_frames;
		
		// skip _lamp_main
		// skip Process::Run()
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
		
		static plus::string Read( const FSTree* parent, const plus::string& name )
		{
			Key pid = GetKeyFromParent( parent );
			
			return Accessor::Get( GetProcess( pid ) );
		}
	};
	
	struct proc_PID_name : readonly_property
	{
		static void get( plus::var_string& result, const FSTree* that, bool binary )
		{
			pid_t pid = GetKeyFromParent( that );
			
			result = GetProcess( pid ).ProgramName();
		}
	};
	
	static FSTreePtr fd_Factory( const FSTree*        parent,
	                             const plus::string&  name,
	                             const void*          args )
	{
		return new FSTree( parent, name, S_IFDIR | 0700, &proc_fd_methods );
	}
	
	static FSTreePtr link_factory( const FSTree*        parent,
	                               const plus::string&  name,
	                               const void*          args )
	{
		return new FSTree( parent, name, S_IFLNK | 0777, &proc_link_methods );
	}
	
	static void proc_pid_core_chmod( const FSTree*  that,
	                                 mode_t         mode )
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
	
	static const node_method_set proc_pid_core_methods =
	{
		NULL,
		&proc_pid_core_chmod
	};
	
	static FSTreePtr core_Factory( const FSTree*        parent,
	                               const plus::string&  name,
	                               const void*          args )
	{
		return new FSTree( parent, name, S_IFREG | 0600, &proc_pid_core_methods );
	}
	
	#define PROPERTY( prop )  &new_property, &property_params_factory< prop >::value
	
	#define GENERATED( gen )  &new_generated, (void*) &proc_PID_Property< gen >::Read
	
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
	
	static FSTreePtr proc_fd_lookup( const FSTree*        that,
	                                 const plus::string&  name,
	                                 const FSTree*        parent )
	{
		const int key = gear::parse_unsigned_decimal( name.c_str() );
		
		const relix::fd_table& sequence = fd_sequence( that );
		
		if ( !sequence.contains( key ) )
		{
			poseven::throw_errno( ENOENT );
		}
		
		return new FSTree( parent,
		                   name,
		                   S_IFLNK | 0777,
		                   &proc_fd_link_methods );
	}
	
	
	static IOHandle* get_proc_fd_handle( const FSTree* that )
	{
		const char* fd_name  = that                  ->name().c_str();
		const char* pid_name = that->owner()->owner()->name().c_str();
		
		const int    fd  = gear::parse_unsigned_decimal( fd_name  );
		const pid_t  pid = gear::parse_unsigned_decimal( pid_name );
		
		relix::fd_table& files = GetProcess( pid ).FileDescriptors();
		
		if ( !files.contains( fd ) )
		{
			p7::throw_errno( ENOENT );
		}
		
		return files.at( fd ).handle.get();
	}
	
	static off_t proc_fd_link_geteof( const FSTree* that )
	{
		IOHandle* handle = get_proc_fd_handle( that );
		
		if ( handle->methods()  &&  handle->methods()->bstore_methods )
		{
			return geteof( *handle );
		}
		
		return 0;
	}
	
	
	class shadow_filehandle : public RegularFileHandle
	{
		private:
			vfs::filehandle_ptr its_basis;
		
		public:
			shadow_filehandle( const vfs::node&  file,
			                   int               flags,
			                   vfs::filehandle&  basis );
			
			~shadow_filehandle();
			
			vfs::filehandle& get() const  { return *its_basis; }
	};
	
	static ssize_t shadow_pread( vfs::filehandle* file, char* buffer, size_t n, off_t offset )
	{
		return pread( static_cast< shadow_filehandle& >( *file ).get(), buffer, n, offset );
	}
	
	static off_t shadow_geteof( vfs::filehandle* file )
	{
		return geteof( static_cast< shadow_filehandle& >( *file ).get() );
	}
	
	static ssize_t shadow_pwrite( vfs::filehandle* file, const char* buffer, size_t n, off_t offset )
	{
		return pwrite( static_cast< shadow_filehandle& >( *file ).get(), buffer, n, offset );
	}
	
	static void shadow_seteof( vfs::filehandle* file, off_t offset )
	{
		seteof( static_cast< shadow_filehandle& >( *file ).get(), offset );
	}
	
	static ssize_t shadow_append( vfs::filehandle* file, const char* buffer, size_t n )
	{
		return append( static_cast< shadow_filehandle& >( *file ).get(), buffer, n );
	}
	
	static const vfs::bstore_method_set shadow_bstore_methods =
	{
		&shadow_pread,
		&shadow_geteof,
		&shadow_pwrite,
		&shadow_seteof,
		&shadow_append,
	};
	
	static const vfs::filehandle_method_set shadow_methods =
	{
		&shadow_bstore_methods,
	};
	
	shadow_filehandle::shadow_filehandle( const vfs::node&  file,
	                                      int               flags,
	                                      vfs::filehandle&  basis )
	:
		RegularFileHandle( &file, flags, &shadow_methods ),
		its_basis( &basis )
	{
	}
	
	shadow_filehandle::~shadow_filehandle()
	{
	}
	
	
	static IOPtr proc_fd_link_open( const FSTree* that, int flags, mode_t mode )
	{
		if ( flags & O_NOFOLLOW )
		{
			p7::throw_errno( ELOOP );
		}
		
		vfs::filehandle_ptr fh = get_proc_fd_handle( that );
		
		if ( fh->methods()  &&  fh->methods()->bstore_methods )
		{
			return new shadow_filehandle( *that, flags, *fh );
		}
		
		return fh;
	}
	
	static FSTreePtr proc_fd_link_resolve( const FSTree* that )
	{
		return get_proc_fd_handle( that )->GetFile();
	}
	
	FSTreePtr New_FSTree_proc( const FSTree*        parent,
	                           const plus::string&  name,
	                           const void*          args )
	{
		return new_basic_directory( parent, name, proc_lookup, proc_iterate );
	}
	
}


/*	==============
 *	FSTree_Proc.cc
 *	==============
 */

#include "Genie/FS/FSTree_Proc.hh"

// Iota
#include "iota/decimal.hh"

// plus
#include "plus/contains.hh"
#include "plus/var_string.hh"

// Recall
#include "recall/backtrace.hh"

// Nitrogen
#include "Nitrogen/Timer.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FS/basic_directory.hh"
#include "Genie/FS/FSTree_Generated.hh"
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/ReadableSymLink.hh"
#include "Genie/FS/ResolvableSymLink.hh"
#include "Genie/IO/Base.hh"
#include "Genie/IO/Device.hh"
#include "Genie/IO/RegularFile.hh"
#include "Genie/IO/Terminal.hh"
#include "Genie/Process.hh"
#include "Genie/Utilities/canonical_positive_integer.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	static pid_t GetKeyFromParent( const FSTreePtr& parent )
	{
		return pid_t( iota::parse_unsigned_decimal( parent->Name().c_str() ) );
	}
	
	static pid_t GetKey( const FSTree* that )
	{
		return GetKeyFromParent( that->ParentRef() );
	}
	
	
	class FSTree_proc_self : public FSTree_ReadableSymLink
	{
		private:
			pid_t getpid() const  { return CurrentProcess().GetPID(); }
		
		public:
			FSTree_proc_self( const FSTreePtr& parent )
			:
				FSTree_ReadableSymLink( parent, "self" )
			{
			}
			
			plus::string ReadLink() const  { return iota::inscribe_unsigned_decimal( getpid() ); }
	};
	
	
	class FSTree_PID_fd : public FSTree_Directory
	{
		private:
			typedef FileDescriptorMap  Sequence;
			
			pid_t its_pid;
		
		public:
			FSTree_PID_fd( const FSTreePtr&     parent,
			               const plus::string&  name,
			               pid_t                pid )
			:
				FSTree_Directory( parent, name ),
				its_pid( pid )
			{
			}
			
			const Sequence& ItemSequence() const
			{
				return GetProcess( its_pid ).FileDescriptors();
			}
			
			FSTreePtr Lookup_Child( const plus::string& name, const FSTree* parent ) const;
			
			void IterateIntoCache( FSTreeCache& cache ) const;
	};
	
	class fdmap_IteratorConverter
	{
		public:
			FSNode operator()( const FileDescriptorMap::value_type& value ) const
			{
				const int key = value.first;
				
				const ino_t inode = key;
				
				plus::string name = iota::inscribe_decimal( key );
				
				return FSNode( inode, name );
			}
	};
	
	void FSTree_PID_fd::IterateIntoCache( FSTreeCache& cache ) const
	{
		fdmap_IteratorConverter converter;
		
		const Sequence& sequence = ItemSequence();
		
		std::transform( sequence.begin(),
		                sequence.end(),
		                std::back_inserter( cache ),
		                converter );
	}
	
	
	class FSTree_PID_fd_N : public FSTree_ResolvableSymLink
	{
		private:
			pid_t  itsPID;
			int    itsFD;
		
		public:
			FSTree_PID_fd_N( const FSTreePtr&     parent,
			                 const plus::string&  name,
			                 pid_t                pid,
			                 int                  fd )
			:
				FSTree_ResolvableSymLink( parent, name ),
				itsPID( pid ),
				itsFD ( fd  )
			{
			}
			
			off_t GetEOF() const;
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags, mode_t mode ) const;
			
			FSTreePtr ResolveLink() const;
	};
	
	class FSTree_PID_Link_Base : public FSTree_ResolvableSymLink
	{
		public:
			FSTree_PID_Link_Base( const FSTreePtr&     parent,
			                      const plus::string&  name )
			:
				FSTree_ResolvableSymLink( parent, name )
			{
			}
			
		protected:
			Process& GetProcess() const;
	};
	
	inline Process& FSTree_PID_Link_Base::GetProcess() const
	{
		const pid_t pid = GetKeyFromParent( ParentRef() );
		
		return Genie::GetProcess( pid );
	}
	
	template < class LinkResolver >
	class FSTree_PID_Link : public FSTree_PID_Link_Base
	{
		public:
			FSTree_PID_Link( const FSTreePtr&     parent,
			                 const plus::string&  name )
			:
				FSTree_PID_Link_Base( parent, name )
			{
			}
			
			FSTreePtr ResolveLink() const  { return LinkResolver::Resolve( GetProcess() ); }
	};
	
	struct ResolveLink_cwd
	{
		static FSTreePtr Resolve( Process& process )
		{
			return process.GetCWD();
		}
	};
	
	struct ResolveLink_exe
	{
		static const FSTreePtr& Resolve( Process& process )
		{
			return process.ProgramFile();
		}
	};
	
	struct ResolveLink_root
	{
		static const FSTreePtr& Resolve( Process& process )
		{
			return FSRoot();
		}
	};
	
	
	extern const FSTree_Premapped::Mapping proc_PID_Mappings[];
	
	template < class Key, class Value >
	static inline bool map_contains( const std::map< Key, Value >& map, const Key& key )
	{
		return map.find( key ) != map.end();
	}
	
	struct valid_name_of_pid
	{
		typedef canonical_positive_integer well_formed_name;
		
		static bool applies( const plus::string& name )
		{
			return    well_formed_name::applies( name )
			       && map_contains( GetProcessList().GetMap(),
			                        int( iota::parse_unsigned_decimal( name.c_str() ) ) );
		}
	};
	
	static FSTreePtr proc_lookup( const FSTreePtr& parent, const plus::string& name )
	{
		if ( name == "self" )
		{
			return seize_ptr( new FSTree_proc_self( parent ) );
		}
		
		if ( !valid_name_of_pid::applies( name ) )
		{
			p7::throw_errno( ENOENT );
		}
		
		return Premapped_Factory< proc_PID_Mappings >( parent, name );
	}
	
	class proc_IteratorConverter
	{
		public:
			FSNode operator()( const ProcessList::Map::value_type& value ) const
			{
				const int key = value.first;
				
				const ino_t inode = key;
				
				plus::string name = iota::inscribe_decimal( key );
				
				return FSNode( inode, name );
			}
	};
	
	static void proc_iterate( FSTreeCache& cache )
	{
		proc_IteratorConverter converter;
		
		const ProcessList::Map& sequence = GetProcessList().GetMap();
		
		std::transform( sequence.begin(),
		                sequence.end(),
		                std::back_inserter( cache ),
		                converter );
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
		else if ( runState == 'S'  &&  N::Microseconds() - process.GetTimeOfLastActivity() > 20 * 1000 * 1000 )
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
				
				const boost::shared_ptr< IOHandle >& term = process.ControllingTerminal();
				
				plus::string terminal_name = "?";
				
				pid_t tpgid = 0;
				
				if ( IOHandle* handle = term.get() )
				{
					terminal_name = handle->GetFile()->Pathname();
					
					TerminalHandle& terminal = IOHandle_Cast< TerminalHandle >( *handle );
					
					const boost::weak_ptr< ProcessGroup >& group = terminal.GetProcessGroup();
					
					tpgid = 0x7fffffff;
					
					if ( !group.expired() )
					{
						tpgid = group.lock()->ID();
					}
				}
				
				plus::var_string result;
				
				result += iota::inscribe_decimal( pid );
				result += " "
				          "(";
				result += process.ProgramName();
				result += ")"
				          " ";
				result += ProcessStateCode( process );
				result += " ";
				result += iota::inscribe_decimal( ppid );
				result += " ";
				result += iota::inscribe_decimal( pgid );
				result += " ";
				result += iota::inscribe_decimal( sid );
				result += " ";
				result += terminal_name;
				result += " ";
				result += iota::inscribe_decimal( tpgid );
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
			return "";
		}
		
		std::vector< frame_data > stackCrawl;
		
		make_stack_crawl( stackCrawl, top );
		
		std::vector< frame_data >::const_iterator begin = stackCrawl.begin();
		std::vector< frame_data >::const_iterator end   = stackCrawl.end();
		
		// skip _lamp_main
		// skip Invoke()
		// skip Run()
		// skip ProcessThreadEntry()
		// skip Adapter()
		// skip ??? (Thread Manager)
		
		if ( end - 6 > begin )
		{
			end -= 6;
		}
		
		plus::var_string result;
		
		make_report_from_stack_crawl( result, begin, end );
		
		return result;
	}
	
	
	template < class Accessor >
	struct proc_PID_Property
	{
		typedef pid_t Key;
		
		static plus::string Read( const FSTree* that )
		{
			Key pid = GetKey( that );
			
			return Accessor::Get( GetProcess( pid ) );
		}
	};
	
	struct proc_PID_name
	{
		static plus::string Read( const FSTree* that, bool binary )
		{
			pid_t pid = GetKey( that );
			
			return GetProcess( pid ).ProgramName();
		}
	};
	
	static FSTreePtr Name_Factory( const FSTreePtr&     parent,
	                               const plus::string&  name )
	{
		FSTreePtr result = New_FSTree_Property( parent,
		                                        name,
		                                        0,
		                                        &proc_PID_name::Read );
		
		return result;
	}
	
	template < class Accessor >
	static FSTreePtr Generated_Factory( const FSTreePtr&     parent,
	                                    const plus::string&  name )
	{
		typedef proc_PID_Property< Accessor > Property;
		
		return New_FSTree_Generated( parent,
		                             name,
		                             &Property::Read );
	}
	
	static FSTreePtr fd_Factory( const FSTreePtr&     parent,
	                             const plus::string&  name )
	{
		const pid_t key = GetKeyFromParent( parent );
		
		return seize_ptr( new FSTree_PID_fd( parent, name, key ) );
	}
	
	template < class LinkResolver >
	FSTreePtr Link_Factory( const FSTreePtr&     parent,
	                        const plus::string&  name )
	{
		return seize_ptr( new FSTree_PID_Link< LinkResolver >( parent, name ) );
	}
	
	class FSTree_proc_PID_core : public FSTree
	{
		public:
			FSTree_proc_PID_core( const FSTreePtr&     parent,
			                      const plus::string&  name )
			:
				FSTree( parent, name )
			{
			}
			
			void ChangeMode( mode_t mode ) const;
	};
	
	void FSTree_proc_PID_core::ChangeMode( mode_t mode ) const
	{
		const pid_t pid = GetKeyFromParent( ParentRef() );
		
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
	
	static FSTreePtr core_Factory( const FSTreePtr&     parent,
	                               const plus::string&  name )
	{
		return seize_ptr( new FSTree_proc_PID_core( parent, name ) );
	}
	
	const FSTree_Premapped::Mapping proc_PID_Mappings[] =
	{
		{ "fd", &fd_Factory },
		
		{ "cwd",  &Link_Factory< ResolveLink_cwd  > },
		{ "exe",  &Link_Factory< ResolveLink_exe  > },
		{ "root", &Link_Factory< ResolveLink_root > },
		
		{ "name", &Name_Factory },
		
		{ "cmdline", &Generated_Factory< proc_PID_cmdline > },
		{ "stat",    &Generated_Factory< proc_PID_stat    > },
		{ "stack",   &Generated_Factory< proc_PID_stack   > },
		
		{ "core", &core_Factory },
		
		{ NULL, NULL }
	};
	
	FSTreePtr FSTree_PID_fd::Lookup_Child( const plus::string& name, const FSTree* parent ) const
	{
		const int key = iota::parse_unsigned_decimal( name.c_str() );
		
		const Sequence& sequence = ItemSequence();
		
		if ( sequence.find( key ) == sequence.end() )
		{
			poseven::throw_errno( ENOENT );
		}
		
		return seize_ptr( new FSTree_PID_fd_N( (parent ? parent : this)->Self(), name, its_pid, key ) );
	}
	
	
	static const boost::shared_ptr< IOHandle >& GetFDHandle( pid_t pid, int fd )
	{
		FileDescriptorMap& files = GetProcess( pid ).FileDescriptors();
		
		FileDescriptorMap::const_iterator it = files.find( fd );
		
		if ( it == files.end() )
		{
			p7::throw_errno( ENOENT );
		}
		
		return it->second.handle;
	}
	
	off_t FSTree_PID_fd_N::GetEOF() const
	{
		IOHandle* handle = GetFDHandle( itsPID, itsFD ).get();
		
		if ( RegularFileHandle* file = IOHandle_Cast< RegularFileHandle >( handle ) )
		{
			return file->GetEOF();
		}
		
		return 0;
	}
	
	boost::shared_ptr< IOHandle > FSTree_PID_fd_N::Open( OpenFlags flags, mode_t mode ) const
	{
		if ( flags & O_NOFOLLOW )
		{
			p7::throw_errno( ELOOP );
		}
		
		return GetFDHandle( itsPID, itsFD )->Clone();
	}
	
	FSTreePtr FSTree_PID_fd_N::ResolveLink() const
	{
		return GetFDHandle( itsPID, itsFD )->GetFile();
	}
	
	FSTreePtr New_FSTree_proc( const FSTreePtr& parent, const plus::string& name )
	{
		return new_basic_directory( parent, name, proc_lookup, proc_iterate );
	}
	
}


/*	==============
 *	FSTree_Proc.cc
 *	==============
 */

#include "Genie/FileSystem/FSTree_Proc.hh"

// Backtrace
#include "Backtrace/Backtrace.hh"

// Nucleus
#include "Nucleus/Convert.h"

// POSeven
#include "POSeven/Errno.hh"

// BitsAndBytes
#include "HexStrings.hh"

// Genie
#include "Genie/FileSystem/FSTree_Directory.hh"
#include "Genie/FileSystem/FSTree_QueryFile.hh"
#include "Genie/IO/Base.hh"
#include "Genie/IO/Device.hh"
#include "Genie/IO/RegularFile.hh"
#include "Genie/IO/Terminal.hh"
#include "Genie/Process.hh"


namespace Genie
{
	
	namespace NN = Nucleus;
	namespace p7 = poseven;
	
	
	struct proc_Details : public Integer_KeyName_Traits< pid_t >
	{
		typedef ProcessList::Map Sequence;
		
		static const Sequence& ItemSequence()  { return GetProcessList().GetMap(); }
		
		static Key KeyFromValue( const Sequence::value_type& value )  { return value.first; }
		
		static bool KeyIsValid( const Key& key )
		{
			const Sequence& sequence = ItemSequence();
			
			return key == 0  ||  sequence.find( key ) != sequence.end();
		}
		
		static FSTreePtr GetChildNode( const FSTreePtr& parent, const Key& key );
	};
	
	typedef FSTree_Sequence< proc_Details > FSTree_proc;
	
	
	struct pid_KeyName_Traits : Integer_KeyName_Traits< pid_t >
	{
	};
	
	typedef Level1_Functional_Details< pid_KeyName_Traits > proc_PID_Details;
	
	class FSTree_proc_PID : public FSTree_Functional< proc_PID_Details >
	{
		private:
			typedef FSTree_Functional< proc_PID_Details > Base;
		
		public:
			FSTree_proc_PID( const FSTreePtr& parent, const Key& key ) : Base( parent, key )
			{
			}
			
			void Init();
			
			std::string Name() const  { return NameFromKey( itsKey ); }
	};
	
	
	class FSTree_proc_self : public FSTree
	{
		private:
			pid_t getpid() const  { return CurrentProcess().GetPID(); }
		
		public:
			FSTree_proc_self( const FSTreePtr& parent ) : FSTree( parent, "self" )
			{
			}
			
			bool IsLink() const  { return true; }
			
			std::string ReadLink() const  { return NN::Convert< std::string >( getpid() ); }
			
			FSTreePtr ResolveLink() const  { return proc_Details::GetChildNode( Parent(), getpid() ); }
	};
	
	
	struct PID_fd_Details : public Integer_KeyName_Traits< int >
	{
		typedef pid_t              ParentKey;
		typedef FileDescriptorMap  Sequence;
		
		ParentKey itsPID;
		
		PID_fd_Details( ParentKey pid ) : itsPID( pid )  {}
		
		const Sequence& ItemSequence() const  { return GetProcess( itsPID ).FileDescriptors(); }
		
		static int KeyFromValue( const Sequence::value_type& value )  { return value.first; }
		
		bool KeyIsValid( const Key& key ) const
		{
			const Sequence& sequence = ItemSequence();
			
			return sequence.find( key ) != sequence.end();
		}
		
		FSTreePtr GetChildNode( const FSTreePtr& parent, const Key& key ) const;
	};
	
	typedef FSTree_Sequence< PID_fd_Details > FSTree_PID_fd;
	
	class FSTree_PID_fd_N : public FSTree
	{
		private:
			pid_t  itsPID;
			int    itsFD;
		
		public:
			FSTree_PID_fd_N( const FSTreePtr&  parent,
			                 pid_t             pid,
			                 int               fd ) : FSTree( parent ),
			                                          itsPID( pid    ),
			                                          itsFD ( fd     )
			{
			}
			
			bool IsLink() const  { return true; }
			
			std::string Name() const;
			
			std::string ReadLink() const  { return ResolveLink()->Pathname(); }
			
			FSTreePtr ResolveLink() const;
	};
	
	class FSTree_PID_Link_Base : public FSTree
	{
		private:
			pid_t itsPID;
		
		public:
			FSTree_PID_Link_Base( const FSTreePtr&    parent,
			                      const std::string&  name,
			                      pid_t               pid ) : FSTree( parent, name ),
			                                                  itsPID( pid )
			{
			}
			
			bool IsLink() const  { return true; }
			
			std::string ReadLink() const  { return ResolveLink()->Pathname(); }
			
		protected:
			Process& GetProcess() const  { return Genie::GetProcess( itsPID ); }
	};
	
	template < class LinkResolver >
	class FSTree_PID_Link : public FSTree_PID_Link_Base
	{
		public:
			FSTree_PID_Link( const FSTreePtr&    parent,
			                 const std::string&  name,
			                 pid_t               pid ) : FSTree_PID_Link_Base( parent, name, pid )
			{
			}
			
			FSTreePtr ResolveLink() const  { return LinkResolver()( GetProcess() ); }
	};
	
	struct ResolveLink_cwd
	{
		FSTreePtr operator()( Process& process ) const
		{
			return process.GetCWD();
		}
	};
	
	struct ResolveLink_exe
	{
		FSTreePtr operator()( Process& process ) const
		{
			return process.ProgramFile();
		}
	};
	
	struct ResolveLink_root
	{
		FSTreePtr operator()( Process& process ) const
		{
			return FSRoot();
		}
	};
	
	
	FSTreePtr GetProcFSTree()
	{
		return GetSingleton< FSTree_proc >( FSRoot(), "proc" );
	}
	
	
	FSTreePtr proc_Details::GetChildNode( const FSTreePtr& parent, const Key& key )
	{
		if ( key == 0 )
		{
			return MakeFSTree( new FSTree_proc_self( parent ) );
		}
		
		return MakeFSTree( new FSTree_proc_PID( parent, key ) );
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
	
	static char ProcessStateCode( ProcessSchedule schedule )
	{
		switch ( schedule )
		{
			case kProcessRunning:      return 'R';  // [1]
			case kProcessSleeping:     return 'S';  // [2]
			case kProcessStopped:      return 'T';  // set in Process::Stop()
			case kProcessFrozen:       return 'F';  // set in SpawnVFork() prior to new Process()
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
	
	class proc_PID_cmdline_Query
	{
		private:
			pid_t itsPID;
		
		public:
			proc_PID_cmdline_Query( pid_t pid ) : itsPID( pid )  {}
			
			std::string operator()() const
			{
				const Process& process = GetProcess( itsPID );
				
				return process.GetCmdLine();
			}
	};
	
	class proc_PID_stat_Query
	{
		private:
			pid_t itsPID;
		
		public:
			proc_PID_stat_Query( pid_t pid ) : itsPID( pid )  {}
			
			std::string operator()() const
			{
				const Process& process = GetProcess( itsPID );
				
				char state_code = ProcessStateCode( process.GetSchedule() );
				
				if ( process.GetLifeStage() == kProcessReleased )
				{
					state_code = 'X';
				}
				
				pid_t ppid = process.GetPPID();
				pid_t pgid = process.GetPGID();
				pid_t sid  = process.GetSID();
				
				const boost::shared_ptr< IOHandle >& term = process.ControllingTerminal();
				
				std::string terminal_name = "?";
				
				pid_t tpgid = 0;
				
				if ( term.get() )
				{
					terminal_name = term->GetFile()->Pathname();
					
					TerminalHandle& terminal = IOHandle_Cast< TerminalHandle >( *term );
					
					const boost::weak_ptr< ProcessGroup >& group = terminal.GetProcessGroup();
					
					tpgid = 0x7fffffff;
					
					if ( !group.expired() )
					{
						tpgid = group.lock()->ID();
					}
				}
				
				return NN::Convert< std::string >( itsPID ) + " "
				       "(" + process.ProgramName() + ")"      " " +
				       state_code                           + " " +
				       NN::Convert< std::string >( ppid   ) + " " +
				       NN::Convert< std::string >( pgid   ) + " " +
				       NN::Convert< std::string >( sid    ) + " " +
				       terminal_name                        + " " +
				       NN::Convert< std::string >( tpgid  ) +
				       "\n";
			}
	};
	
	class proc_PID_backtrace_Query
	{
		private:
			pid_t itsPID;
		
		public:
			proc_PID_backtrace_Query( pid_t pid ) : itsPID( pid )  {}
			
			std::string operator()() const;
	};
	
	std::string proc_PID_backtrace_Query::operator()() const
	{
		using Backtrace::StackFramePtr;
		using Backtrace::ReturnAddress;
		
		const Process& process = GetProcess( itsPID );
		
		StackFramePtr top    = process.GetStackFramePointer();
		StackFramePtr bottom = process.GetStackBottomPointer();
		
		if ( top == NULL )
		{
			return "";
		}
		
		std::vector< ReturnAddress > stackCrawl = MakeStackCrawlFromTopToBottom( top, bottom );
		
		std::vector< ReturnAddress >::const_iterator begin = stackCrawl.begin();
		std::vector< ReturnAddress >::const_iterator end   = stackCrawl.end();
		
		--end;  // skip Genie::Process::Run( void )
		
		std::string result = MakeReportFromStackCrawl( begin, end );
		
		return result;
	}
	
	
	static FSTreePtr fd_Factory( const FSTreePtr&    parent,
	                             const std::string&  name,
	                             pid_t               key )
	{
		return MakeFSTree( new FSTree_PID_fd( parent, name, key ) );
	}
	
	template < class LinkResolver >
	FSTreePtr Link_Factory( const FSTreePtr&    parent,
	                        const std::string&  name,
	                        pid_t               key )
	{
		return MakeFSTree( new FSTree_PID_Link< LinkResolver >( parent, name, key ) );
	}
	
	template < class Query >
	FSTreePtr Query_Factory( const FSTreePtr&    parent,
	                         const std::string&  name,
	                         pid_t               key )
	{
		typedef FSTree_QueryFile< Query > QueryFile;
		
		return MakeFSTree( new QueryFile( parent, name, Query( key ) ) );
	}
	
	void FSTree_proc_PID::Init()
	{
		Map( "fd", &fd_Factory );
		
		Map( "cwd",  &Link_Factory< ResolveLink_cwd  > );
		Map( "exe",  &Link_Factory< ResolveLink_exe  > );
		Map( "root", &Link_Factory< ResolveLink_root > );
		
		Map( "cmdline",   &Query_Factory< proc_PID_cmdline_Query   > );
		Map( "stat",      &Query_Factory< proc_PID_stat_Query      > );
		Map( "backtrace", &Query_Factory< proc_PID_backtrace_Query > );
	}
	
	FSTreePtr PID_fd_Details::GetChildNode( const FSTreePtr& parent, const Key& key ) const
	{
		return MakeFSTree( new FSTree_PID_fd_N( parent, itsPID, key ) );
	}
	
	
	class FSTree_MagicFileReference : public FSTree
	{
		private:
			typedef boost::shared_ptr< IOHandle > Key;
			
			Key itsHandle;
		
		public:
			FSTree_MagicFileReference( const Key& io ) : itsHandle( io )  {}
			
			std::string Name    () const  { return itsHandle->GetFile()->Name    (); }
			std::string Pathname() const  { return itsHandle->GetFile()->Pathname(); }
			
			FSTreePtr Parent() const  { return itsHandle->GetFile()->Parent(); }
			
			void Stat( struct ::stat& sb ) const  { itsHandle->GetFile()->Stat( sb ); }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags /*flags*/ ) const
			{
				return itsHandle;
			}
			
			off_t GetEOF() const;
	};
	
	off_t FSTree_MagicFileReference::GetEOF() const
	{
		return IOHandle_Cast< RegularFileHandle >( *itsHandle.get() ).GetEOF();
	}
	
	std::string FSTree_PID_fd_N::Name() const
	{
		return NN::Convert< std::string >( itsFD );
	}
	
	FSTreePtr FSTree_PID_fd_N::ResolveLink() const
	{
		FileDescriptorMap& files = GetProcess( itsPID ).FileDescriptors();
		
		if ( files.find( itsFD ) == files.end() )
		{
			p7::throw_errno( ENOENT );
		}
		
		return FSTreePtr( new FSTree_MagicFileReference( files[ itsFD ].handle ) );
	}
	
}


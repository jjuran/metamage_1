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
#include "Genie/Process.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	namespace NN = Nucleus;
	namespace p7 = poseven;
	
	
	struct proc_Details
	{
		typedef GenieProcessTable Sequence;
		
		static std::string Name()  { return "proc"; }
		
		static FSTreePtr Lookup( const std::string& name );
		
		static const Sequence& ItemSequence()  { return gProcessTable; }
		
		static std::string ChildName( const Sequence::value_type& child )
		{
			return NN::Convert< std::string >( child.first );
		}
		
		static FSTreePtr ChildNode( const Sequence::value_type& child );
	};
	
	typedef FSTree_Special_Unique< proc_Details > FSTree_proc;
	
	
	class FSTree_PID : public FSTree_Virtual
	{
		private:
			typedef pid_t Key;
			
			Key itsPID;
		
		public:
			FSTree_PID( Key pid );
			
			std::string Name() const  { return NN::Convert< std::string >( itsPID ); }
			
			FSTreePtr Parent() const  { return GetProcFSTree(); }
	};
	
	
	class FSTree_proc_self : public FSTree
	{
		private:
			pid_t getpid() const  { return CurrentProcess().GetPID(); }
		
		public:
			bool IsLink() const  { return true; }
			
			std::string Name() const  { return "self"; }
			
			FSTreePtr Parent() const  { return GetProcFSTree(); }
			
			std::string ReadLink() const  { return NN::Convert< std::string >( getpid() ); }
			
			FSTreePtr ResolveLink() const  { return FSTreePtr( new FSTree_PID( getpid() ) ); }
	};
	
	
	struct PID_fd_Details;
	
	typedef FSTree_Special< PID_fd_Details > FSTree_PID_fd;
	
	struct PID_fd_Details
	{
		typedef pid_t              Key;
		typedef FileDescriptorMap  Sequence;
		
		Key itsPID;
		
		PID_fd_Details( Key pid ) : itsPID( pid )  {}
		
		static std::string Name()  { return "fd"; }
		
		FSTreePtr Parent() const  { return FSTreePtr( new FSTree_PID   ( itsPID ) ); }
		
		FSTreePtr Lookup( const std::string& name ) const;
		
		const Sequence& ItemSequence() const  { return GetProcess( itsPID ).FileDescriptors(); }
		
		static std::string ChildName( const Sequence::value_type& child )
		{
			return NN::Convert< std::string >( child.first );
		}
		
		FSTreePtr ChildNode( const Sequence::value_type& child ) const;
	};
	
	class FSTree_PID_fd_N : public FSTree
	{
		private:
			pid_t  itsPID;
			int    itsFD;
		
		public:
			FSTree_PID_fd_N( pid_t pid, int fd ) : itsPID( pid ), itsFD( fd )  {}
			
			bool IsLink() const  { return true; }
			
			std::string Name() const;
			
			FSTreePtr Parent() const  { return FSTreePtr( new FSTree_PID_fd( itsPID ) ); }
			
			std::string ReadLink() const  { return ResolveLink()->Pathname(); }
			
			FSTreePtr ResolveLink() const;
	};
	
	class FSTree_PID_cwd : public FSTree
	{
		private:
			pid_t itsPID;
		
		public:
			FSTree_PID_cwd( pid_t pid ) : itsPID( pid )  {}
			
			bool IsLink() const  { return true; }
			
			std::string Name() const  { return "cwd"; }
			
			FSTreePtr Parent() const  { return FSTreePtr( new FSTree_PID( itsPID ) ); }
			
			std::string ReadLink() const  { return ResolveLink()->Pathname(); }
			
			FSTreePtr ResolveLink() const  { return GetProcess( itsPID ).GetCWD(); }
	};
	
	class FSTree_PID_exe : public FSTree
	{
		private:
			pid_t itsPID;
		
		public:
			FSTree_PID_exe( pid_t pid ) : itsPID( pid )  {}
			
			bool IsLink() const  { return true; }
			
			std::string Name() const  { return "exe"; }
			
			FSTreePtr Parent() const  { return FSTreePtr( new FSTree_PID( itsPID ) ); }
			
			std::string ReadLink() const  { return ResolveLink()->Pathname(); }
			
			FSTreePtr ResolveLink() const  { return GetProcess( itsPID ).ProgramFile(); }
	};
	
	class FSTree_PID_root : public FSTree
	{
		private:
			pid_t itsPID;
		
		public:
			FSTree_PID_root( pid_t pid ) : itsPID( pid )  {}
			
			bool IsLink() const  { return true; }
			
			std::string Name() const  { return "root"; }
			
			FSTreePtr Parent() const  { return FSTreePtr( new FSTree_PID( itsPID ) ); }
			
			std::string ReadLink() const  { return ResolveLink()->Pathname(); }
			
			//FSTreePtr ResolveLink() const  { return FSTreeFromFSSpec( GetProcess( itsPID ).RootDirectory() ); }
			FSTreePtr ResolveLink() const  { return FSRoot(); }
	};
	
	
	FSTreePtr GetProcFSTree()
	{
		return GetSingleton< FSTree_proc >();
	}
	
	
	FSTreePtr proc_Details::Lookup( const std::string& name )
	{
		if ( name == "self" )
		{
			return FSTreePtr( new FSTree_proc_self() );
		}
		
		pid_t pid = std::atoi( name.c_str() );
		
		return FSTreePtr( new FSTree_PID( pid ) );
	}
	
	FSTreePtr proc_Details::ChildNode( const Sequence::value_type& child )
	{
		return FSTreePtr( new FSTree_PID( child.first ) );
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
			case kProcessStopped:      return '.';  // set in Process::Stop()
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
				
				pid_t ppid = process.GetPPID();
				pid_t pgid = process.GetPGID();
				pid_t sid = process.GetSID();
				
				return NN::Convert< std::string >( itsPID ) + " "
				       "(" + process.ProgramName() + ")"      " " +
				       ProcessStateCode( process.GetSchedule() ) + " " +
				       NN::Convert< std::string >( ppid   ) + " " +
				       NN::Convert< std::string >( pgid   ) + " " +
				       NN::Convert< std::string >( sid    ) + " "
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
		const Process& process = GetProcess( itsPID );
		
		Backtrace::StackFramePtr top = process.GetStackFramePointer();
		
		std::vector< Backtrace::ReturnAddress > stackCrawl = MakeStackCrawl( top );
		
		std::vector< Backtrace::CallInfo > callChain( stackCrawl.size() );
		
		std::transform( stackCrawl.begin(),
		                stackCrawl.end(),
		                callChain.begin(),
		                std::ptr_fun( Backtrace::GetCallInfoFromReturnAddress ) );
		
		std::string result;
		
		unsigned offset = 0;
		
		typedef std::vector< Backtrace::CallInfo >::const_iterator Iter;
		
		for ( Iter it = callChain.begin();  it != callChain.end();  ++it, ++offset )
		{
			using BitsAndBytes::EncodeAsHex;
			
			result += NN::Convert< std::string >( offset );
			result += ": 0x";
			result += EncodeAsHex( it->itsReturnAddr );
			result += " (";
			result += it->itsArch;
			result += ") ";
			result += it->itsUnmangledName;
			result += "\n";
			
			if ( it->itsUnmangledName == "main" )
			{
				break;
			}
		}
		
		return result;
	}
	
	
	FSTree_PID::FSTree_PID( pid_t pid ) : itsPID( pid )
	{
		Map( FSTreePtr( new FSTree_PID_fd  ( pid ) ) );
		Map( FSTreePtr( new FSTree_PID_cwd ( pid ) ) );
		Map( FSTreePtr( new FSTree_PID_exe ( pid ) ) );
		Map( FSTreePtr( new FSTree_PID_root( pid ) ) );
		
		Map( FSTreePtr( new FSTree_QueryFile< proc_PID_cmdline_Query >( Pathname(),
		                                                                "cmdline",
		                                                                proc_PID_cmdline_Query( pid ) ) ) );
		
		Map( FSTreePtr( new FSTree_QueryFile< proc_PID_stat_Query >( Pathname(),
		                                                             "stat",
		                                                             proc_PID_stat_Query( pid ) ) ) );
		
		Map( FSTreePtr( new FSTree_QueryFile< proc_PID_backtrace_Query >( Pathname(),
		                                                                  "backtrace",
		                                                                  proc_PID_backtrace_Query( pid ) ) ) );
	}
	
	FSTreePtr PID_fd_Details::Lookup( const std::string& name ) const
	{
		int fd = std::atoi( name.c_str() );
		
		const FileDescriptorMap& files = GetProcess( itsPID ).FileDescriptors();
		
		if ( files.find( fd ) == files.end() )
		{
			p7::throw_errno( ENOENT );
		}
		
		return FSTreePtr( new FSTree_PID_fd_N( itsPID, fd ) );
	}
	
	FSTreePtr PID_fd_Details::ChildNode( const Sequence::value_type& child ) const
	{
		int fd = child.first;
		
		return FSTreePtr( new FSTree_PID_fd_N( itsPID, fd ) );
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


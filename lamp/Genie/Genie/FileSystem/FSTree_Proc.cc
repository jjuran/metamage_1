/*	==============
 *	FSTree_Proc.cc
 *	==============
 */

#include "Genie/FileSystem/FSTree_Proc.hh"

// Nucleus
#include "Nucleus/Convert.h"

// POSeven
#include "POSeven/Errno.hh"

// Genie
#include "Genie/FileSystem/FSTree_Directory.hh"
#include "Genie/IO/Base.hh"
#include "Genie/Process.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	namespace NN = Nucleus;
	namespace P7 = POSeven;
	
	
	struct proc_Details
	{
		static std::string Name()  { return "proc"; }
		
		static FSTreePtr Lookup( const std::string& name );
		
		static const GenieProcessTable& ItemSequence()  { return gProcessTable; }
		
		static FSNode ConvertToFSNode( GenieProcessTable::ProcessMap::value_type proc );
		
		FSNode operator()( GenieProcessTable::ProcessMap::value_type proc ) const  { return ConvertToFSNode( proc ); }
	};
	
	typedef FSTree_Special_Unique< proc_Details > FSTree_proc;
	
	
	class FSTree_PID : public FSTree_Virtual
	{
		private:
			pid_t itsPID;
		
		public:
			FSTree_PID( pid_t pid );
			
			std::string Name() const;
			
			FSTreePtr Parent() const  { return GetProcFSTree(); }
	};
	
	
	class FSTree_proc_self : public FSTree
	{
		private:
			pid_t getpid() const  { return CurrentProcess().ProcessID(); }
		
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
		pid_t itsPID;
		
		PID_fd_Details( pid_t pid ) : itsPID( pid )  {}
		
		static std::string Name()  { return "fd"; }
		
		FSTreePtr Parent() const  { return FSTreePtr( new FSTree_PID   ( itsPID ) ); }
		
		FSTreePtr Lookup( const std::string& name ) const;
		
		const FileDescriptorMap& ItemSequence() const  { return gProcessTable[ itsPID ].FileDescriptors(); }
		
		FSNode ConvertToFSNode( FileDescriptorMap::value_type file ) const;
		
		FSNode operator()( FileDescriptorMap::value_type file ) const  { return ConvertToFSNode( file ); }
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
			
			std::string ReadLink() const  { return GetFSTreePathname( ResolveLink() ); }
			
			FSTreePtr ResolveLink() const  { return gProcessTable[ itsPID ].CurrentWorkingDirectory(); }
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
			
			std::string ReadLink() const  { return GetFSTreePathname( ResolveLink() ); }
			
			FSTreePtr ResolveLink() const  { return FSTreeFromFSSpec( gProcessTable[ itsPID ].ProgramFile() ); }
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
			
			std::string ReadLink() const  { return GetFSTreePathname( ResolveLink() ); }
			
			//FSTreePtr ResolveLink() const  { return FSTreeFromFSSpec( gProcessTable[ itsPID ].RootDirectory() ); }
			FSTreePtr ResolveLink() const  { return FSRoot(); }
	};
	
	
	FSTreePtr GetProcFSTree()
	{
		static FSTreePtr proc = FSTreePtr( new FSTree_proc() );
		
		return proc;
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
	
	FSNode proc_Details::ConvertToFSNode( GenieProcessTable::ProcessMap::value_type proc )
	{
		pid_t pid = proc.first;
		
		std::string name = NN::Convert< std::string >( pid );
		
		FSTreePtr tree( new FSTree_PID( pid ) );
		
		return FSNode( name, tree );
	}
	
	
	FSTree_PID::FSTree_PID( pid_t pid ) : itsPID( pid )
	{
		Map( "fd",   FSTreePtr( new FSTree_PID_fd  ( pid ) ) );
		Map( "cwd",  FSTreePtr( new FSTree_PID_cwd ( pid ) ) );
		Map( "exe",  FSTreePtr( new FSTree_PID_exe ( pid ) ) );
		Map( "root", FSTreePtr( new FSTree_PID_root( pid ) ) );
	}
	
	std::string FSTree_PID::Name() const
	{
		return NN::Convert< std::string >( itsPID );
	}
	
	FSTreePtr PID_fd_Details::Lookup( const std::string& name ) const
	{
		int fd = std::atoi( name.c_str() );
		
		const FileDescriptorMap& files = gProcessTable[ itsPID ].FileDescriptors();
		
		if ( files.find( fd ) == files.end() )
		{
			P7::ThrowErrno( ENOENT );
		}
		
		return FSTreePtr( new FSTree_PID_fd_N( itsPID, fd ) );
	}
	
	FSNode PID_fd_Details::ConvertToFSNode( FileDescriptorMap::value_type file ) const
	{
		int fd = file.first;
		
		std::string name = NN::Convert< std::string >( fd );
		
		FSTreePtr tree( new FSTree_PID_fd_N( itsPID, fd ) );
		
		return FSNode( name, tree );
	}
	
	
	class FSTree_MagicFileReference : public FSTree
	{
		private:
			boost::shared_ptr< IOHandle > itsHandle;
		
		public:
			FSTree_MagicFileReference( const boost::shared_ptr< IOHandle >& io ) : itsHandle( io )  {}
			
			std::string Name    () const  { return itsHandle->GetFile()->Name    (); }
			std::string Pathname() const  { return itsHandle->GetFile()->Pathname(); }
			
			FSTreePtr Parent() const  { return itsHandle->GetFile()->Parent(); }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const
			{
				return itsHandle;
			}
	};
	
	std::string FSTree_PID_fd_N::Name() const
	{
		return NN::Convert< std::string >( itsFD );
	}
	
	FSTreePtr FSTree_PID_fd_N::ResolveLink() const
	{
		FileDescriptorMap& files = gProcessTable[ itsPID ].FileDescriptors();
		
		if ( files.find( itsFD ) == files.end() )
		{
			P7::ThrowErrno( ENOENT );
		}
		
		return FSTreePtr( new FSTree_MagicFileReference( files[ itsFD ].handle ) );
	}
	
}


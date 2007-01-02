/*	==============
 *	FSTree_Proc.cc
 *	==============
 */

#include "Genie/FileSystem/FSTree_Proc.hh"

// Nucleus
#include "Nucleus/Convert.h"

// Genie
#include "Genie/FileSystem/FSTree_Directory.hh"
#include "Genie/Process.hh"


namespace Genie
{
	
	namespace NN = Nucleus;
	
	
	struct proc_Details
	{
		static std::string Name()  { return "proc"; }
		
		static FSTreePtr Lookup( const std::string& name );
		
		static const GenieProcessTable& ItemSequence()  { return gProcessTable; }
		
		static FSNode ConvertToFSNode( GenieProcessTable::ProcessMap::value_type proc );
	};
	
	typedef FSTree_Special< proc_Details > FSTree_proc;
	
	
	class FSTree_pid : public FSTree_Directory
	{
		private:
			pid_t itsPID;
		
		public:
			FSTree_pid( pid_t pid ) : itsPID( pid )  {}
			
			std::string Name() const;
			
			FSTreePtr Self()   const;
			FSTreePtr Parent() const;
			
			FSTreePtr Lookup_Child( const std::string& name ) const;
			
			void IterateIntoCache( FSTreeCache& cache ) const;
	};
	
	
	FSTreePtr GetProcFSTree()
	{
		static FSTreePtr proc = FSTreePtr( new FSTree_proc() );
		
		return proc;
	}
	
	
	FSTreePtr proc_Details::Lookup( const std::string& name )
	{
		pid_t pid = std::atoi( name.c_str() );
		
		return FSTreePtr( new FSTree_pid( pid ) );
	}
	
	FSNode proc_Details::ConvertToFSNode( GenieProcessTable::ProcessMap::value_type proc )
	{
		pid_t pid = proc.first;
		
		std::string name = NN::Convert< std::string >( pid );
		
		FSTreePtr tree( new FSTree_pid( pid ) );
		
		return FSNode( name, tree );
	}
	
	
	std::string FSTree_pid::Name() const
	{
		return NN::Convert< std::string >( itsPID );
	}
	
	FSTreePtr FSTree_pid::Self() const
	{
		return FSTreePtr( new FSTree_pid( *this ) );
	}
	
	FSTreePtr FSTree_pid::Parent() const
	{
		return FSTreePtr( new FSTree_proc() );
	}
	
	FSTreePtr FSTree_pid::Lookup_Child( const std::string& name ) const
	{
		pid_t pid = std::atoi( name.c_str() );
		
		return FSNull();
	}
	
	void FSTree_pid::IterateIntoCache( FSTreeCache& cache ) const
	{
		//
	}
	
}


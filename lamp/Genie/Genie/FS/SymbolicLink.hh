/*
	SymbolicLink.hh
	---------------
*/

#ifndef GENIE_FS_SYMBOLICLINK_HH
#define GENIE_FS_SYMBOLICLINK_HH

// POSIX
#include <sys/stat.h>

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	class FSTree_SymbolicLink : public FSTree
	{
		private:
			plus::string itsTarget;
		
		public:
			FSTree_SymbolicLink( const FSTreePtr&     parent,
			                     const plus::string&  name,
			                     const plus::string&  target );
			
			const plus::string& Target() const  { return itsTarget; }
	};
	
	FSTreePtr New_FSTree_SymbolicLink( const FSTreePtr&     parent,
	                                   const plus::string&  name,
	                                   const plus::string&  target );
	
}

#endif


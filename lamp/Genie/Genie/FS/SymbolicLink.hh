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
			typedef void (*remove_method)( const FSTree* );
			
			remove_method its_remove;
			
			plus::string itsTarget;
		
		public:
			FSTree_SymbolicLink( const FSTreePtr&     parent,
			                     const plus::string&  name,
			                     const plus::string&  target,
			                     remove_method        remove = NULL );
			
			void Delete() const;
			
			const plus::string& Target() const  { return itsTarget; }
	};
	
	FSTreePtr New_FSTree_SymbolicLink( const FSTreePtr&     parent,
	                                   const plus::string&  name,
	                                   const plus::string&  target );
	
}

#endif


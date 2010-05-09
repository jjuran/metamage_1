/*
	SymbolicLink.cc
	---------------
*/

#include "Genie/FS/SymbolicLink.hh"

// Genie
#include "Genie/FS/ReadableSymLink.hh"
#include "Genie/FS/ResolvePathname.hh"


namespace Genie
{
	
	class FSTree_SymbolicLink : public FSTree_ReadableSymLink
	{
		private:
			plus::string itsTarget;
		
		public:
			FSTree_SymbolicLink( const FSTreePtr&     parent,
			                     const plus::string&  name,
			                     const plus::string&  target )
			:
				FSTree_ReadableSymLink( parent, name ),
				itsTarget( target )
			{
			}
			
			plus::string ReadLink() const  { return itsTarget; }
	};
	
	FSTreePtr New_FSTree_SymbolicLink( const FSTreePtr&     parent,
	                                   const plus::string&  name,
	                                   const plus::string&  target )
	{
		return seize_ptr( new FSTree_SymbolicLink( parent, name, target ) );
	}
	
}


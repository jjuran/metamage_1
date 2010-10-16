/*
	SymbolicLink.hh
	---------------
*/

#ifndef GENIE_FS_SYMBOLICLINK_HH
#define GENIE_FS_SYMBOLICLINK_HH

// Genie
#include "Genie/FS/ReadableSymLink.hh"


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
			
			plus::string ReadLink() const;
	};
	
	FSTreePtr New_FSTree_SymbolicLink( const FSTreePtr&     parent,
	                                   const plus::string&  name,
	                                   const plus::string&  target );
	
}

#endif


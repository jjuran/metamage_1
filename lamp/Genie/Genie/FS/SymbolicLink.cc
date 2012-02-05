/*
	SymbolicLink.cc
	---------------
*/

#include "Genie/FS/SymbolicLink.hh"


namespace Genie
{
	
	FSTree_SymbolicLink::FSTree_SymbolicLink( const FSTreePtr&     parent,
	                                          const plus::string&  name,
	                                          const plus::string&  target )
	:
		FSTree_ReadableSymLink( parent, name ),
		itsTarget( target )
	{
	}
	
	plus::string FSTree_SymbolicLink::ReadLink() const
	{
		return itsTarget;
	}
	
	FSTreePtr New_FSTree_SymbolicLink( const FSTreePtr&     parent,
	                                   const plus::string&  name,
	                                   const plus::string&  target )
	{
		return new FSTree_SymbolicLink( parent, name, target );
	}
	
}


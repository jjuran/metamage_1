/*
	SymbolicLink.cc
	---------------
*/

#include "Genie/FS/SymbolicLink.hh"


namespace Genie
{
	
	plus::string FSTree_SymbolicLink::ReadLink() const
	{
		return itsTarget;
	}
	
	FSTreePtr New_FSTree_SymbolicLink( const FSTreePtr&     parent,
	                                   const plus::string&  name,
	                                   const plus::string&  target )
	{
		return seize_ptr( new FSTree_SymbolicLink( parent, name, target ) );
	}
	
}


/*
	SymbolicLink.cc
	---------------
*/

#include "Genie/FS/SymbolicLink.hh"

// Genie
#include "Genie/FS/node_method_set.hh"


namespace Genie
{
	
	static plus::string readlink( const FSTree* node )
	{
		const FSTree_SymbolicLink* file = static_cast< const FSTree_SymbolicLink* >( node );
		
		return file->Target();
	}
	
	static const node_method_set methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&readlink
	};
	
	FSTree_SymbolicLink::FSTree_SymbolicLink( const FSTreePtr&     parent,
	                                          const plus::string&  name,
	                                          const plus::string&  target )
	:
		FSTree( parent, name, S_IFLNK | 0777, &methods ),
		itsTarget( target )
	{
	}
	
	FSTreePtr New_FSTree_SymbolicLink( const FSTreePtr&     parent,
	                                   const plus::string&  name,
	                                   const plus::string&  target )
	{
		return new FSTree_SymbolicLink( parent, name, target );
	}
	
}


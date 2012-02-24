/*
	SymbolicLink.cc
	---------------
*/

#include "Genie/FS/SymbolicLink.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FS/link_method_set.hh"
#include "Genie/FS/node_method_set.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	static void remove( const FSTree* node )
	{
		const FSTree_SymbolicLink* file = static_cast< const FSTree_SymbolicLink* >( node );
		
		return file->Delete();
	}
	
	static plus::string readlink( const FSTree* node )
	{
		const FSTree_SymbolicLink* file = static_cast< const FSTree_SymbolicLink* >( node );
		
		return file->Target();
	}
	
	static const link_method_set link_methods =
	{
		&readlink
	};
	
	static const node_method_set methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		&remove,
		NULL,
		NULL,
		&link_methods
	};
	
	FSTree_SymbolicLink::FSTree_SymbolicLink( const FSTreePtr&     parent,
	                                          const plus::string&  name,
	                                          const plus::string&  target,
	                                          remove_method        remove )
	:
		FSTree( parent, name, S_IFLNK | 0777, &methods ),
		its_remove( remove ),
		itsTarget( target )
	{
	}
	
	void FSTree_SymbolicLink::Delete() const
	{
		if ( its_remove )
		{
			its_remove( this );
		}
		else
		{
			p7::throw_errno( EPERM );
		}
	}
	
	FSTreePtr New_FSTree_SymbolicLink( const FSTreePtr&     parent,
	                                   const plus::string&  name,
	                                   const plus::string&  target )
	{
		return new FSTree_SymbolicLink( parent, name, target );
	}
	
}


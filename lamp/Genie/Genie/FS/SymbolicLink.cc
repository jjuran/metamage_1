/*
	SymbolicLink.cc
	---------------
*/

#include "Genie/FS/SymbolicLink.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Genie
#include "Genie/FS/link_method_set.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	struct symlink_extra
	{
		plus::datum_storage  target;
		remove_method        remove;
	};
	
	static void remove( const FSTree* node )
	{
		symlink_extra& extra = *(symlink_extra*) node->extra();
		
		if ( extra.remove )
		{
			extra.remove( node );
		}
		else
		{
			p7::throw_errno( EPERM );
		}
	}
	
	static plus::string readlink( const FSTree* node )
	{
		symlink_extra& extra = *(symlink_extra*) node->extra();
		
		return reinterpret_cast< const plus::string& >( extra.target );
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
	
	static void dispose_symlink( const FSTree* node )
	{
		symlink_extra& extra = *(symlink_extra*) node->extra();
		
		plus::destroy( extra.target );
	}
	
	FSTree_SymbolicLink::FSTree_SymbolicLink( const FSTreePtr&     parent,
	                                          const plus::string&  name,
	                                          plus::string         target,
	                                          remove_method        remove )
	:
		FSTree( parent,
		        name,
		        S_IFLNK | 0777,
		        &methods,
		        sizeof (symlink_extra),
		        &dispose_symlink )
	{
		symlink_extra& extra = *(symlink_extra*) this->extra();
		
		plus::construct_from_move( extra.target, target.move() );
		
		extra.remove = remove;
	}
	
	FSTreePtr New_FSTree_SymbolicLink( const FSTreePtr&     parent,
	                                   const plus::string&  name,
	                                   const plus::string&  target,
	                                   remove_method        remove )
	{
		return new FSTree_SymbolicLink( parent, name, target, remove );
	}
	
}


/*
	symbolic_link.cc
	----------------
*/

#include "vfs/node/types/symbolic_link.hh"

// POSIX
#include <sys/stat.h>

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/methods/link_method_set.hh"
#include "vfs/methods/node_method_set.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	struct symlink_extra
	{
		vxo::datum_storage  target;
		remove_method       remove;
	};
	
	static void remove( const node* that )
	{
		symlink_extra& extra = *(symlink_extra*) that->extra();
		
		if ( extra.remove )
		{
			extra.remove( that );
		}
		else
		{
			p7::throw_errno( EPERM );
		}
	}
	
	static plus::string readlink( const node* that )
	{
		symlink_extra& extra = *(symlink_extra*) that->extra();
		
		return reinterpret_cast< const plus::string& >( extra.target );
	}
	
	static const item_method_set item_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		&remove,
	};
	
	static const link_method_set link_methods =
	{
		&readlink
	};
	
	static const node_method_set methods =
	{
		&item_methods,
		NULL,
		&link_methods
	};
	
	static void dispose_symlink( const node* that )
	{
		symlink_extra& extra = *(symlink_extra*) that->extra();
		
		vxo::destroy( extra.target );
	}
	
	node_ptr new_symbolic_link( const node*          parent,
	                            const plus::string&  name,
	                            plus::string         target,
	                            remove_method        remove )
	{
		node* result = new node( parent,
		                         name,
		                         S_IFLNK | 0777,
		                         &methods,
		                         sizeof (symlink_extra),
		                         &dispose_symlink );
		
		symlink_extra& extra = *(symlink_extra*) result->extra();
		
		vxo::construct_from_move( extra.target, target.move() );
		
		extra.remove = remove;
		
		return result;
	}
	
}

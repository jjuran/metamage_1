/*
	null.cc
	-------
*/

#include "vfs/nodes/null.hh"

// plus
#include "plus/string.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/methods/misc_method_set.hh"
#include "vfs/methods/node_method_set.hh"


namespace vfs
{
	
	namespace p7 = poseven;
	
	
	static node_ptr null_parent( const node* it )
	{
		throw p7::errno_t( ENOENT );
	}
	
	static const misc_method_set null_misc_methods =
	{
		&null_parent
	};
	
	static const node_method_set null_methods =
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
		&null_misc_methods
	};
	
	node_ptr null()
	{
		return new node( NULL, plus::string::null, 0, &null_methods );
	}
	
}


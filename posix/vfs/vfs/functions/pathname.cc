/*
	pathname.cc
	-----------
*/

#include "vfs/functions/pathname.hh"

// plus
#include "plus/string/concat.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/node_ptr.hh"
#include "vfs/primitives/parent.hh"


namespace vfs
{
	
	plus::string pathname( const node& that )
	{
		plus::string pathname = that.name();
		
		node_ptr tree = parent( that );
		
		if ( tree.get() == &that  &&  !pathname.empty() )
		{
			// Anonymous files have null parents (but so does root)
			return pathname;
		}
		
		while ( true )
		{
			const plus::string& name = tree->name();
			
			pathname = name + "/" + pathname;
			
			// Root dir has empty name
			
			if ( name.empty() )
			{
				break;
			}
			
			tree = parent( *tree );
		}
		
		return pathname;
	}
	
}


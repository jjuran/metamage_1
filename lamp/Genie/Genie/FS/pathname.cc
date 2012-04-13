/*
	pathname.cc
	-----------
*/

#include "Genie/FS/pathname.hh"

// plus
#include "plus/string/concat.hh"

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/parent.hh"


namespace Genie
{
	
	plus::string pathname( const FSTree* it )
	{
		plus::string pathname = it->name();
		
		FSTreePtr tree = parent( it );
		
		if ( tree.get() == it  &&  !pathname.empty() )
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
			
			tree = parent( tree.get() );
		}
		
		return pathname;
	}
	
}


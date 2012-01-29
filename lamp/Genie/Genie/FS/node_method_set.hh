/*
	node_method_set.hh
	------------------
*/

#ifndef GENIE_FS_NODEMETHODSET_HH
#define GENIE_FS_NODEMETHODSET_HH

// plus
#include "plus/string.hh"


namespace Genie
{
	
	class FSTree;
	
	typedef void (*symlink_method)( const FSTree*, const plus::string& );
	
	
	struct node_method_set
	{
		symlink_method  symlink;
	};
	
}

#endif


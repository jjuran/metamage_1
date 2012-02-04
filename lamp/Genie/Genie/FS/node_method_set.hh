/*
	node_method_set.hh
	------------------
*/

#ifndef GENIE_FS_NODEMETHODSET_HH
#define GENIE_FS_NODEMETHODSET_HH

// plus
#include "plus/string.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	class FSTree;
	
	typedef FSTreePtr (*parent_method)( const FSTree* );
	
	typedef void (*symlink_method)( const FSTree*, const plus::string& );
	
	
	struct node_method_set
	{
		parent_method   parent;
		symlink_method  symlink;
	};
	
}

#endif


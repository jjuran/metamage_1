/*
	file_method_set.hh
	------------------
*/

#ifndef GENIE_FS_FILEMETHODSET_HH
#define GENIE_FS_FILEMETHODSET_HH

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	class FSTree;
	
	typedef void (*attach_method)( const FSTree*, const FSTreePtr& );
	
	
	struct file_method_set
	{
		attach_method  attach;
	};
	
}

#endif


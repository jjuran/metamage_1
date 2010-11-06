/*
	ResolvableSymLink.cc
	--------------------
*/

#include "Genie/FS/ResolvableSymLink.hh"

// POSIX
#include <sys/stat.h>


namespace Genie
{
	
	FSTree_ResolvableSymLink::FSTree_ResolvableSymLink( const FSTreePtr&     parent,
	                                                    const plus::string&  name )
	:
		FSTree( parent, name, S_IFLNK | 0777 )
	{
	}
	
	plus::string FSTree_ResolvableSymLink::ReadLink() const
	{
		return ResolveLink()->Pathname();
	}
	
}


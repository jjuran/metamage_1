/*
	ReadableSymLink.cc
	------------------
	
	Copyright 2009, Joshua Juran
*/

#include "Genie/FS/ReadableSymLink.hh"

// POSIX
#include <sys/stat.h>

// Genie
#include "Genie/FS/ResolvePathname.hh"


namespace Genie
{
	
	FSTree_ReadableSymLink::FSTree_ReadableSymLink( const FSTreePtr&        parent,
	                                                const plus::string&     name,
	                                                const node_method_set*  methods )
	:
		FSTree( parent, name, S_IFLNK | 0777, methods )
	{
	}
	
	FSTreePtr FSTree_ReadableSymLink::ResolveLink() const
	{
		return ResolvePathname( ReadLink(), ParentRef() );
	}
	
}


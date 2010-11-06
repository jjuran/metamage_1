/*
	ReadableSymLink.cc
	------------------
	
	Copyright 2009, Joshua Juran
*/

#include "Genie/FS/ReadableSymLink.hh"

// Genie
#include "Genie/FS/ResolvePathname.hh"


namespace Genie
{
	
	FSTree_ReadableSymLink::FSTree_ReadableSymLink( const FSTreePtr&     parent,
	                                                const plus::string&  name )
	:
		FSTree( parent, name )
	{
	}
	
	FSTreePtr FSTree_ReadableSymLink::ResolveLink() const
	{
		return ResolvePathname( ReadLink(), ParentRef() );
	}
	
}


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
	
	FSTreePtr FSTree_ReadableSymLink::ResolveLink() const
	{
		return ResolvePathname( ReadLink(), ParentRef() );
	}
	
}


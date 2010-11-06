/*
	ResolvableSymLink.cc
	--------------------
*/

#include "Genie/FS/ResolvableSymLink.hh"


namespace Genie
{
	
	FSTree_ResolvableSymLink::FSTree_ResolvableSymLink( const FSTreePtr&     parent,
	                                                    const plus::string&  name )
	:
		FSTree( parent, name )
	{
	}
	
	plus::string FSTree_ResolvableSymLink::ReadLink() const
	{
		return ResolveLink()->Pathname();
	}
	
}


/*
	ResolvableSymLink.cc
	--------------------
*/

#include "Genie/FS/ResolvableSymLink.hh"


namespace Genie
{
	
	plus::string FSTree_ResolvableSymLink::ReadLink() const
	{
		return ResolveLink()->Pathname();
	}
	
}


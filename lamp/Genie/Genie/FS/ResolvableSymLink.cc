/*
	ResolvableSymLink.cc
	--------------------
*/

#include "Genie/FS/ResolvableSymLink.hh"


namespace Genie
{
	
	std::string FSTree_ResolvableSymLink::ReadLink() const
	{
		return ResolveLink()->Pathname();
	}
	
}


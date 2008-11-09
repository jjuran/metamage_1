/*	======================
 *	FSTree_Virtual_Link.cc
 *	======================
 */

#include "Genie/FileSystem/FSTree_Virtual_Link.hh"

// Genie
#include "Genie/FileSystem/ResolvePathname.hh"


namespace Genie
{
	
	FSTreePtr FSTree_Virtual_Link::ResolveLink() const
	{
		return ResolvePathname( itsTarget );
	}
	
}


/*	===================
 *	FSTree_QueryFile.cc
 *	===================
 */

#include "Genie/FileSystem/FSTree_QueryFile.hh"

// Genie
#include "Genie/FileSystem/ResolvePathname.hh"


namespace Genie
{
	
	FSTreePtr FSTree_QueryFile_Base::Parent() const
	{
		return ResolvePathname( itsParentPathname, FSRoot() );
	}
	
}


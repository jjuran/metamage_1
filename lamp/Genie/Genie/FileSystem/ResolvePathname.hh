/*	==================
 *	ResolvePathname.hh
 *	==================
 */

#ifndef GENIE_FILESYSTEM_RESOLVEPATHNAME_HH
#define GENIE_FILESYSTEM_RESOLVEPATHNAME_HH

// Standard C++
#include <string>

// Genie
#include "Genie/FileSystem/FSTree.hh"


namespace Genie
{
	
	bool ResolveLink_InPlace ( FSTreePtr& link );
	bool ResolveLinks_InPlace( FSTreePtr& link );
	
	FSTreePtr ResolvePathname( const std::string& pathname, const FSTreePtr& current = FSTreePtr() );
	
}

#endif


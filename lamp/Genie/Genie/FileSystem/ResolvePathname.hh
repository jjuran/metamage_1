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
	
	FSTreePtr ResolvePathname( const std::string& pathname, FSTreePtr current );
	
}

#endif


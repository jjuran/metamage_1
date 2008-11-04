/*	================
 *	ResolvePathAt.hh
 *	================
 */

#ifndef GENIE_FILESYSTEM_RESOLVEPATHAT_HH
#define GENIE_FILESYSTEM_RESOLVEPATHAT_HH

// Standard C++
#include <string>

// Genie
#include "Genie/FileSystem/FSTree.hh"


namespace Genie
{
	
	FSTreePtr ResolvePathAt( int dirfd, const std::string& path );
	
}

#endif


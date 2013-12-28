/*	================
 *	ResolvePathAt.hh
 *	================
 */

#ifndef GENIE_FILESYSTEM_RESOLVEPATHAT_HH
#define GENIE_FILESYSTEM_RESOLVEPATHAT_HH

// plus
#include "plus/string.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	vfs::node_ptr ResolvePathAt( int dirfd, const plus::string& path );
	
}

#endif


/*	==================
 *	ResolvePathname.hh
 *	==================
 */

#ifndef GENIE_FILESYSTEM_RESOLVEPATHNAME_HH
#define GENIE_FILESYSTEM_RESOLVEPATHNAME_HH

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	bool ResolveLinks_InPlace( FSTreePtr& link );
	
}

#endif


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
	
	FSTreePtr ResolveRelativePath( const char*    begin,
	                               std::size_t    length,
	                               const FSTree*  current );
	
}

#endif


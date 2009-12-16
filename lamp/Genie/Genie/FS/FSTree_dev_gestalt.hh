/*	=====================
 *	FSTree_dev_gestalt.hh
 *	=====================
 */

#ifndef GENIE_FILESYSTEM_FSTREEDEVGESTALT_HH
#define GENIE_FILESYSTEM_FSTREEDEVGESTALT_HH

// POSIX
#include "sys/stat.h"

// Genie
#include "Genie/IO/Base.hh"


namespace Genie
{
	
	struct dev_gestalt
	{
		static const mode_t perm = S_IRUSR;
		
		static boost::shared_ptr< IOHandle > open( OpenFlags flags );
	};
	
}

#endif


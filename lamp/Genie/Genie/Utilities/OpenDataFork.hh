/*
	OpenDataFork.hh
	---------------
*/

#ifndef GENIE_UTILITIES_OPENDATAFORK_HH
#define GENIE_UTILITIES_OPENDATAFORK_HH

// nucleus
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
#endif

// Nitrogen
#ifndef MAC_FILES_TYPES_FSFILEREFNUM_HH
#include "Mac/Files/Types/FSFileRefNum.hh"
#endif
#ifndef MAC_FILES_TYPES_FSIOPERM_HH
#include "Mac/Files/Types/FSIOPerm.hh"
#endif


namespace Genie
{
	
	nucleus::owned< Mac::FSFileRefNum > OpenDataFork( const FSSpec&  file,
	                                                  Mac::FSIOPerm  perm );
	
}

#endif

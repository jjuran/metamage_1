/*	=================
 *	FSSpecForkUser.hh
 *	=================
 */

#ifndef GENIE_FILESYSTEM_FSSPECFORKUSER_HH
#define GENIE_FILESYSTEM_FSSPECFORKUSER_HH

// nucleus
#ifndef NUCLEUS_SHARED_HH
#include "nucleus/shared.hh"
#endif

// Nitrogen
#ifndef MAC_FILES_TYPES_FSFILEREFNUM_HH
#include "Mac/Files/Types/FSFileRefNum.hh"
#endif
#ifndef MAC_FILES_TYPES_FSIOPERM_HH
#include "Mac/Files/Types/FSIOPerm.hh"
#endif

// Genie
#include "Genie/IO/Base.hh"


namespace Genie
{
	
	typedef nucleus::owned< Mac::FSFileRefNum > (*ForkOpener)( const FSSpec&, Mac::FSIOPerm );
	
	typedef boost::shared_ptr< IOHandle > (*HandleCreator)( const nucleus::shared< Mac::FSFileRefNum >&, OpenFlags );
	
	boost::shared_ptr< IOHandle >
	//
	OpenMacFileHandle( const FSSpec&  fileSpec,
	                   OpenFlags      flags,
	                   ForkOpener     openFork,
	                   HandleCreator  createHandle );
	
}

#endif


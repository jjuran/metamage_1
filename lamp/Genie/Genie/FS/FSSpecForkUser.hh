/*	=================
 *	FSSpecForkUser.hh
 *	=================
 */

#ifndef GENIE_FILESYSTEM_FSSPECFORKUSER_HH
#define GENIE_FILESYSTEM_FSSPECFORKUSER_HH

// nucleus
#include "nucleus/shared.hh"

// Nitrogen
#include "Mac/Files/Types/FSFileRefNum.hh"
#include "Mac/Files/Types/FSIOPerm.hh"

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


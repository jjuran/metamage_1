/*	=================
 *	FSSpecForkUser.hh
 *	=================
 */

#ifndef GENIE_FILESYSTEM_FSSPECFORKUSER_HH
#define GENIE_FILESYSTEM_FSSPECFORKUSER_HH

// nucleus
#include "nucleus/shared.hh"

// Nitrogen
#include "Nitrogen/Files.hh"

// Genie
#include "Genie/IO/Base.hh"


namespace Genie
{
	
	typedef nucleus::owned< Nitrogen::FSFileRefNum > (*ForkOpener)( const FSSpec&, Nitrogen::FSIOPermissions );
	
	typedef boost::shared_ptr< IOHandle > (*HandleCreator)( const nucleus::shared< Nitrogen::FSFileRefNum >&, OpenFlags );
	
	boost::shared_ptr< IOHandle >
	//
	OpenMacFileHandle( const FSSpec&  fileSpec,
	                   OpenFlags      flags,
	                   ForkOpener     openFork,
	                   HandleCreator  createHandle );
	
}

#endif


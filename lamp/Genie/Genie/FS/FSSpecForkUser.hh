/*	=================
 *	FSSpecForkUser.hh
 *	=================
 */

#ifndef GENIE_FILESYSTEM_FSSPECFORKUSER_HH
#define GENIE_FILESYSTEM_FSSPECFORKUSER_HH

// Nucleus
#include "Nucleus/Shared.h"

// Nitrogen
#include "Nitrogen/Files.hh"

// Genie
#include "Genie/IO/Base.hh"


namespace Genie
{
	
	typedef Nucleus::Owned< Nitrogen::FSFileRefNum > (*ForkOpener)( const FSSpec&, Nitrogen::FSIOPermissions );
	
	typedef boost::shared_ptr< IOHandle > (*HandleCreator)( const Nucleus::Shared< Nitrogen::FSFileRefNum >&, OpenFlags );
	
	boost::shared_ptr< IOHandle >
	//
	OpenMacFileHandle( const FSSpec&  fileSpec,
	                   OpenFlags      flags,
	                   ForkOpener     openFork,
	                   HandleCreator  createHandle );
	
}

#endif


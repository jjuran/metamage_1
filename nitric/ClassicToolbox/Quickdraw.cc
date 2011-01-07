/*	============
 *	Quickdraw.cc
 *	============
 */

#include "ClassicToolbox/Quickdraw.hh"

// MacGlue
#include "MacGlue/MacGlue.hh"


namespace MacGlue
{
	
	DECLARE_MAC_GLUE( ClosePort  );
	DECLARE_MAC_GLUE( CloseCPort );
	
}

namespace Nitrogen
{
	
	nucleus::owned< GrafPtr > OpenPort( GrafPtr port )
	{
		::OpenPort( port );
		
		return nucleus::owned< GrafPtr >::seize( port, &MacGlue::ClosePort );
	}
	
	nucleus::owned< CGrafPtr > OpenCPort( CGrafPtr port )
	{
		::OpenCPort( port );
		
		return nucleus::owned< CGrafPtr >::seize( port, &MacGlue::CloseCPort );
	}
	
}


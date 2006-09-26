/*	============
 *	Quickdraw.cp
 *	============
 */

#ifndef CLASSICTOOLBOX_QUICKDRAW_H
#include "ClassicToolbox/Quickdraw.h"
#endif


namespace Nitrogen
{
	
	Nucleus::Owned< GrafPtr > OpenPort( GrafPtr port )
	{
		::OpenPort( port );
		
		return Nucleus::Owned< GrafPtr >::Seize( port, &::ClosePort );
	}
	
	Nucleus::Owned< CGrafPtr > OpenCPort( CGrafPtr port )
	{
		::OpenCPort( port );
		
		return Nucleus::Owned< CGrafPtr >::Seize( port, &::CloseCPort );
	}
	
}


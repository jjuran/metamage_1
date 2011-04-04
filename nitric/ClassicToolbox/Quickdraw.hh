/*	============
 *	Quickdraw.hh
 *	============
 */

#ifndef CLASSICTOOLBOX_QUICKDRAW_HH
#define CLASSICTOOLBOX_QUICKDRAW_HH

// Nitrogen
#ifndef NITROGEN_QUICKDRAW_HH
#include "Nitrogen/Quickdraw.hh"
#endif


namespace Nitrogen
{
	
	nucleus::owned< GrafPtr > OpenPort( GrafPtr port );
	
	inline void ClosePort( nucleus::owned< GrafPtr > )  {}
	
	nucleus::owned< CGrafPtr > OpenCPort( CGrafPtr port );
	
	inline void CloseCPort( nucleus::owned< CGrafPtr > )  {}
	
}

#endif


/*	===========
 *	Quickdraw.h
 *	===========
 */

#ifndef CLASSICTOOLBOX_QUICKDRAW_H
#define CLASSICTOOLBOX_QUICKDRAW_H

// Nitrogen
#ifndef NITROGEN_QUICKDRAW_H
#include "Nitrogen/QuickDraw.h"
#endif


namespace Nitrogen
{
	
#if CALL_NOT_IN_CARBON
	
	using ::InitGraf;
	
	Nucleus::Owned< GrafPtr > OpenPort( GrafPtr port );
	
	using ::InitPort;
	
	inline void ClosePort( Nucleus::Owned< GrafPtr > )  {}
	
	Nucleus::Owned< CGrafPtr > OpenCPort( CGrafPtr port );
	
	using ::InitCPort;
	
	inline void CloseCPort( Nucleus::Owned< CGrafPtr > )  {}
	
#endif  // CALL_NOT_IN_CARBON
	
}

#endif


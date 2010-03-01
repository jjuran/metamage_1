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
	
	using ::InitGraf;
	
	nucleus::owned< GrafPtr > OpenPort( GrafPtr port );
	
	using ::InitPort;
	
	inline void ClosePort( nucleus::owned< GrafPtr > )  {}
	
	nucleus::owned< CGrafPtr > OpenCPort( CGrafPtr port );
	
	using ::InitCPort;
	
	inline void CloseCPort( nucleus::owned< CGrafPtr > )  {}
	
}

#endif


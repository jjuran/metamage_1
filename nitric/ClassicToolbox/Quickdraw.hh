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
	
	Nucleus::Owned< GrafPtr > OpenPort( GrafPtr port );
	
	using ::InitPort;
	
	inline void ClosePort( Nucleus::Owned< GrafPtr > )  {}
	
	Nucleus::Owned< CGrafPtr > OpenCPort( CGrafPtr port );
	
	using ::InitCPort;
	
	inline void CloseCPort( Nucleus::Owned< CGrafPtr > )  {}
	
}

#endif


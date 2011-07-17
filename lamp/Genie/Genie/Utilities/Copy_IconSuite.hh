/*
	Copy_IconSuite.hh
	-----------------
*/

#ifndef GENIE_UTILITIES_GETMACHINENAME_HH
#define GENIE_UTILITIES_GETMACHINENAME_HH

// Nitrogen
#ifndef NITROGEN_ICONS_HH
#include "Nitrogen/Icons.hh"
#endif


namespace Genie
{
	
	nucleus::owned< Nitrogen::IconSuiteRef > Copy_IconSuite( Nitrogen::IconSuiteRef iconSuite );
	
}

#endif


/*
	Mac/Toolbox/Types/Boolean.hh
	----------------------------
*/

#ifndef MAC_TOOLBOX_TYPES_BOOLEAN_HH
#define MAC_TOOLBOX_TYPES_BOOLEAN_HH

// Mac OS
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

// nucleus
#ifndef NUCLEUS_SCRIBE_HH
#include "nucleus/scribe.hh"
#endif


namespace Mac
{
	
	typedef nucleus::converting_POD_scribe< bool, ::Boolean > Boolean_scribe;
	
}

#endif


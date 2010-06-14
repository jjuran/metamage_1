/*
	Mac/Files/Types/FSCreator.hh
	----------------------------
*/

#ifndef MAC_FILES_TYPES_FSCREATOR_HH
#define MAC_FILES_TYPES_FSCREATOR_HH

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

// nucleus
#include "nucleus/enumeration_traits.hh"


namespace Mac
{
	
	enum FSCreator
	{
		kUnknownFSCreator = ::kUnknownType,
		
		kFSCreator_Max = nucleus::enumeration_traits< ::OSType >::max
	};
	
}

#endif


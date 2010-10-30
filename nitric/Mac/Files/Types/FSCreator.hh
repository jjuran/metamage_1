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
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif


namespace Mac
{
	
	enum FSCreator
	{
		kUnknownFSCreator = ::kUnknownType,
		
		kHFSPlusCreator = 'hfs+',  // for hard links
		kSymLinkCreator = 'rhap',
		
		kFSCreator_Max = nucleus::enumeration_traits< ::OSType >::max
	};
	
}

#endif


/*
	Mac/Files/Types/FSType.hh
	-------------------------
*/

#ifndef MAC_FILES_TYPES_FSTYPE_HH
#define MAC_FILES_TYPES_FSTYPE_HH

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

// nucleus
#include "nucleus/enumeration_traits.hh"


namespace Mac
{
	
	enum FSType
	{
		kUnknownFSType = ::kUnknownType,
		
		kHardLinkFileType = 'hlnk',
		kSymLinkFileType  = 'slnk',
		
		kFSType_Max = nucleus::enumeration_traits< ::OSType >::max
	};
	
}

#endif


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
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif


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


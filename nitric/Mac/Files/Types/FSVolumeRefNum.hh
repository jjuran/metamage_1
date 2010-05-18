/*
	Mac/Files/Types/FSVolumeRefNum.hh
	---------------------------------
*/

#ifndef MAC_FILES_TYPES_FSVOLUMEREFNUM_HH
#define MAC_FILES_TYPES_FSVOLUMEREFNUM_HH

#ifndef __FILES__
#include <Files.h>
#endif

// nucleus
#include "nucleus/enumeration_traits.hh"


namespace Mac
{
	
	enum FSVolumeRefNum
	{
		kFSVolumeRefNum_Max = nucleus::enumeration_traits< ::FSVolumeRefNum >::max
	};
	
}

#endif


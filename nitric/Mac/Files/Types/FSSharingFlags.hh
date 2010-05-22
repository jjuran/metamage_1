/*
	Mac/Files/Types/FSSharingFlags.hh
	---------------------------------
*/

#ifndef MAC_FILES_TYPES_FSSHARINGFLAGS_HH
#define MAC_FILES_TYPES_FSSHARINGFLAGS_HH

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

// nucleus
#include "nucleus/enumeration_traits.hh"
#include "nucleus/flag_ops.hh"


namespace Mac
{
	
	enum FSSharingFlags
	{
		kFSSharingFlags_Max = nucleus::enumeration_traits< UInt8 >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( FSSharingFlags )
	
	typedef FSSharingFlags FSIOFileAttributes;
	
}

#endif


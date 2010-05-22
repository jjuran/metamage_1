/*
	Mac/Files/Types/FSUserPrivileges.hh
	-----------------------------------
*/

#ifndef MAC_FILES_TYPES_FSUSERPRIVILEGES_HH
#define MAC_FILES_TYPES_FSUSERPRIVILEGES_HH

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

// nucleus
#include "nucleus/enumeration_traits.hh"
#include "nucleus/flag_ops.hh"


namespace Mac
{
	
	enum FSUserPrivileges
	{
		kFSUserPrivileges_Max = nucleus::enumeration_traits< UInt8 >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( FSUserPrivileges )
	
}

#endif


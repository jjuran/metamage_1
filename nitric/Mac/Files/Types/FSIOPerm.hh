/*
	Mac/Files/Types/FSIOPerm.hh
	---------------------------
*/

#ifndef MAC_FILES_TYPES_FSIOPERM_HH
#define MAC_FILES_TYPES_FSIOPERM_HH

#ifndef __FILES__
#include <Files.h>
#endif

// nucleus
#include "nucleus/enumeration_traits.hh"
#include "nucleus/flag_ops.hh"


namespace Mac
{
	
	enum FSIOPerm
	{
		fsCurPerm    = ::fsCurPerm,
		fsRdPerm     = ::fsRdPerm,
		fsWrPerm     = ::fsWrPerm,
		fsRdWrPerm   = ::fsRdWrPerm,
		fsRdWrShPerm = ::fsRdWrShPerm,
		fsRdDenyPerm = ::fsRdDenyPerm,
		fsWrDenyPerm = ::fsWrDenyPerm,
		
		kFSIOPerm_Max = nucleus::enumeration_traits< SInt8 >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( FSIOPerm )
	
	typedef FSIOPerm FSIOPermssn, FSIOPermissions;
	
}

#endif


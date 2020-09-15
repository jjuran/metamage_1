/*
	Mac/Files/Types/FSRefNameSpec.hh
	--------------------------------
*/

#ifndef MAC_FILES_TYPES_FSREFNAMESPEC_HH
#define MAC_FILES_TYPES_FSREFNAMESPEC_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif


namespace Mac
{
	
	struct FSRefNameSpec
	{
		FSRef         parent;
		HFSUniStr255  name;
	};
	
}

#endif

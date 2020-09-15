/*
	FindFolder.hh
	-------------
*/

#ifndef MACSYS_HAS_FINDFOLDER_HH
#define MACSYS_HAS_FINDFOLDER_HH

// mac-sys-utils
#include "mac_sys/gestalt.hh"


namespace mac {
namespace sys {
	
	inline
	bool has_FindFolder()
	{
		enum
		{
			gestaltFindFolderAttr = 'fold',
		};
		
		return gestalt( gestaltFindFolderAttr );
	}
	
}
}

#endif

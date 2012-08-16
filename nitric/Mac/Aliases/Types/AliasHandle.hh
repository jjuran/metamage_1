/*
	Mac/Aliases/Types/AliasHandle.hh
	--------------------------------
*/

#ifndef MAC_ALIASES_TYPES_ALIASHANDLE_HH
#define MAC_ALIASES_TYPES_ALIASHANDLE_HH

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __ALIASES__
#include <Aliases.h>
#endif

// Nitrogen
#ifndef MAC_MEMORY_TYPES_HANDLE_HH
#include "Mac/Memory/Types/Handle.hh"
#endif


namespace nucleus
{
	
	template <>
	struct disposer_class< AliasHandle > : disposer_class< Mac::Handle >
	{
	};
	
}

#endif


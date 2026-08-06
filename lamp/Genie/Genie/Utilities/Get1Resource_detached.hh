/*
	Get1Resource_detached.hh
	------------------------
*/

#ifndef GENIE_UTILITIES_GET1RESOURCEDETACHED_HH
#define GENIE_UTILITIES_GET1RESOURCEDETACHED_HH

// nucleus
#ifndef NUCLEUS_OWNED_HH
#include "nucleus/owned.hh"
#endif

// Nitrogen
#ifndef MAC_MEMORY_TYPES_HANDLE_HH
#include "Mac/Memory/Types/Handle.hh"
#endif


namespace Genie
{
	
	typedef char** Handle;
	
	nucleus::owned< Mac::Handle > Get1Resource_detached( ResType t, short id );
	
}

#endif

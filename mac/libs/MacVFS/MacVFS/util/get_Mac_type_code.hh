/*
	get_Mac_type_code.hh
	--------------------
*/

#ifndef MACVFS_UTIL_GETMACTYPECODE_HH
#define MACVFS_UTIL_GETMACTYPECODE_HH

// vfs
#include "vfs/node_fwd.hh"


namespace vfs
{
	
	unsigned long get_Mac_type_code( const node& file );
	
}

#endif

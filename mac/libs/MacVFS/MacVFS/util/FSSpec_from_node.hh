/*
	FSSpec_from_node.hh
	-------------------
*/

#ifndef MACVFS_UTIL_FSSPECFROMNODE_HH
#define MACVFS_UTIL_FSSPECFROMNODE_HH

// mac-types
#include "mac_types/FSSpec.hh"

// vfs
#include "vfs/node_fwd.hh"


namespace vfs
{
	
	mac::types::FSSpec FSSpec_from_node( const node& file );
	
}

#endif

/*
	make_alias.hh
	-------------
*/

#ifndef RELIX_API_MAKEALIAS_HH
#define RELIX_API_MAKEALIAS_HH

// vfs
#include "vfs/node_fwd.hh"


namespace relix
{
	
	void make_alias( const vfs::node& target, const vfs::node& alias );
	
}

#endif

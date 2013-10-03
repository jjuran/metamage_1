/*
	make_alias.hh
	-------------
*/

#ifndef GENIE_KERNEL_MAKEALIAS_HH
#define GENIE_KERNEL_MAKEALIAS_HH

// vfs
#include "vfs/node_fwd.hh"


namespace Genie
{
	
	void make_alias( const vfs::node& target, const vfs::node& alias );
	
}

#endif


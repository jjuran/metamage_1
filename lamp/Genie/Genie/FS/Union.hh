/*
	Union.hh
	--------
	
	Copyright 2010, Joshua Juran
*/

#ifndef GENIE_FS_UNION_HH
#define GENIE_FS_UNION_HH

// plus
#include "plus/string_fwd.hh"

// vfs
#include "vfs/node_ptr.hh"


namespace Genie
{
	
	vfs::node_ptr New_FSTree_Union( const vfs::node*     parent,
	                                const plus::string&  name,
	                                const vfs::node*     top,
	                                const vfs::node*     bottom );
	
}

#endif


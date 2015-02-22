/*
	Volumes.hh
	----------
	
	Copyright 2010, Joshua Juran
*/

#ifndef GENIE_FS_VOLUMES_HH
#define GENIE_FS_VOLUMES_HH

// plus
#include "plus/string.hh"

// vfs
#include "vfs/node_ptr.hh"


namespace Genie
{
	
	vfs::node_ptr New_FSTree_Volumes( const vfs::node*     parent,
	                                  const plus::string&  name,
	                                  const void*          args );
	
}

#endif

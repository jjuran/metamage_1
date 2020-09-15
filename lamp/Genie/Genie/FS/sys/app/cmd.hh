/*
	Genie/FS/sys/app/cmd.hh
	-----------------------
*/

#ifndef GENIE_FS_APP_HH
#define GENIE_FS_APP_HH

// plus
#include "plus/string.hh"

// vfs
#include "vfs/node_ptr.hh"


namespace Genie
{
	
	vfs::node_ptr New_FSTree_sys_app_cmd( const vfs::node*     parent,
	                                      const plus::string&  name,
	                                      const void*          args );
	
}

#endif

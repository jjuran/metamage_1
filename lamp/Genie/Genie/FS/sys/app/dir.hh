/*
	Genie/FS/sys/app/dir.hh
	-----------------------
*/

#ifndef GENIE_FS_SYS_APP_DIR_HH
#define GENIE_FS_SYS_APP_DIR_HH

// plus
#include "plus/string.hh"

// vfs
#include "vfs/node_ptr.hh"


namespace Genie
{
	
	vfs::node_ptr New_FSTree_sys_app_dir( const vfs::node*     parent,
	                                      const plus::string&  name,
	                                      const void*          args );
	
}

#endif

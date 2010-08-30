/*
	Genie/FS/sys/app/dir.hh
	-----------------------
*/

#ifndef GENIE_FS_SYS_APP_DIR_HH
#define GENIE_FS_SYS_APP_DIR_HH

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	FSTreePtr New_FSTree_sys_app_dir( const FSTreePtr&     parent,
	                                  const plus::string&  name,
	                                  const void*          args );
	
}

#endif


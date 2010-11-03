/*
	Genie/FS/sys/app/cmd.hh
	-----------------------
*/

#ifndef GENIE_FS_APP_HH
#define GENIE_FS_APP_HH

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	FSTreePtr New_FSTree_sys_app_cmd( const FSTreePtr&     parent,
	                                  const plus::string&  name,
	                                  const void*          args );
	
}

#endif


/*
	Genie/FS/sys/app/cmd.hh
	-----------------------
*/

#ifndef GENIE_FS_APP_HH
#define GENIE_FS_APP_HH

// plus
#include "plus/string.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	FSTreePtr New_FSTree_sys_app_cmd( const FSTree*        parent,
	                                  const plus::string&  name,
	                                  const void*          args );
	
}

#endif


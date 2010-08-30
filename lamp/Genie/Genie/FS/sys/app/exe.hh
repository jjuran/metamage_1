/*
	Genie/FS/sys/app/exe.hh
	-----------------------
*/

#ifndef GENIE_FS_SYS_APP_EXE_HH
#define GENIE_FS_SYS_APP_EXE_HH

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	FSTreePtr New_FSTree_sys_app_exe( const FSTreePtr&     parent,
	                                  const plus::string&  name,
	                                  const void*          args );
	
}

#endif


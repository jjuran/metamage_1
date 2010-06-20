/*
	sys_app_exe.hh
	--------------
	
	Copyright 2009, Joshua Juran
*/

#ifndef GENIE_FS_SYSAPPEXE_HH
#define GENIE_FS_SYSAPPEXE_HH

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	FSTreePtr New_FSTree_sys_app_exe( const FSTreePtr&     parent,
	                                  const plus::string&  name,
	                                  const void*          args );
	
}

#endif


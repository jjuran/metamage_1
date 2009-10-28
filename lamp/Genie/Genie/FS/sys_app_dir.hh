/*
	sys_app_dir.hh
	--------------
	
	Copyright 2009, Joshua Juran
*/

#ifndef GENIE_FS_SYSAPPDIR_HH
#define GENIE_FS_SYSAPPDIR_HH

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	FSTreePtr New_FSTree_sys_app_dir( const FSTreePtr& parent, const std::string& name );
	
}

#endif


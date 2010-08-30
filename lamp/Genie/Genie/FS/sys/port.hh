/*
	Genie/FS/sys/port.hh
	--------------------
*/

#ifndef GENIE_FS_SYS_PORT_HH
#define GENIE_FS_SYS_PORT_HH

// Genie
#include "Genie/FS/FSTree.hh"


namespace Genie
{
	
	FSTreePtr NewWindow();
	
	void RemoveWindow( const FSTree* window );
	
	
	FSTreePtr New_FSTree_sys_window( const FSTreePtr&     parent,
	                                 const plus::string&  name,
	                                 const void*          args );
	
}

#endif


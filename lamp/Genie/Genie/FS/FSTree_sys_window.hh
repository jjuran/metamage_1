/*	====================
 *	FSTree_sys_window.hh
 *	====================
 */

#ifndef GENIE_FILESYSTEM_FSTREESYSWINDOW_HH
#define GENIE_FILESYSTEM_FSTREESYSWINDOW_HH

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


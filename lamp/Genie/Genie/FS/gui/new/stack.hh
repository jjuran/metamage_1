/*
	gui/new/stack.cc
	----------------
*/

#ifndef GENIE_FS_GUI_NEW_STACK_HH
#define GENIE_FS_GUI_NEW_STACK_HH

// plus
#include "plus/string_fwd.hh"

// vfs
#include "vfs/node_ptr.hh"


namespace Genie
{
	
	vfs::node_ptr New_stack( const vfs::node*     parent,
	                         const plus::string&  name,
	                         const void*          args );
	
}

#endif

/*
	gui/new/stack.cc
	----------------
*/

#ifndef GENIE_FS_GUI_NEW_STACK_HH
#define GENIE_FS_GUI_NEW_STACK_HH

// plus
#include "plus/string_fwd.hh"

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace Genie
{
	
	FSTreePtr New_stack( const FSTree*        parent,
	                     const plus::string&  name,
	                     const void*          args );
	
}

#endif


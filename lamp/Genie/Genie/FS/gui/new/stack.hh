/*
	gui/new/stack.cc
	----------------
*/

#ifndef GENIE_FS_GUI_NEW_STACK_HH
#define GENIE_FS_GUI_NEW_STACK_HH

// Genie
#include "Genie/FS/FSTreePtr.hh"


namespace plus
{
	
	class string;
	
}

namespace Genie
{
	
	FSTreePtr New_stack( const FSTreePtr&     parent,
	                     const plus::string&  name,
	                     const void*          args );
	
}

#endif


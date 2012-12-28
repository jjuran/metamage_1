/*
	gui/new/progress.hh
	-------------------
*/

#ifndef GENIE_FS_GUI_NEW_PROGRESS_HH
#define GENIE_FS_GUI_NEW_PROGRESS_HH

// plus
#include "plus/string_fwd.hh"

// vfs
#include "vfs/node_ptr.hh"


namespace Genie
{
	
	vfs::node_ptr New_new_progress( const vfs::node*     parent,
	                                const plus::string&  name,
	                                const void*          args );
	
}

#endif


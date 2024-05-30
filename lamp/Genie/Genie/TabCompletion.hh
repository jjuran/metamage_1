/*
	TabCompletion.hh
	----------------
*/

#ifndef GENIE_TABCOMPLETION_HH
#define GENIE_TABCOMPLETION_HH

// plus
#include "plus/var_string.hh"

// vfs
#include "vfs/node.hh"


namespace Genie
{
	
	enum TabCompletion
	{
		TabCompletion_failed = -1,
		TabCompletion_stuck  = 0,
		TabCompletion_done   = 1,
	};
	
	TabCompletion tab_complete( const vfs::node*   dir,
	                            plus::var_string&  s,
	                            int                i,
	                            int                j );
	
}

#endif

/*
	focusable_views.hh
	------------------
*/

#ifndef GENIE_FS_FOCUSABLEVIEWS_HH
#define GENIE_FS_FOCUSABLEVIEWS_HH

// vfs
#include "vfs/node_fwd.hh"


namespace Pedestal
{
	
	class View;
	
}

namespace Genie
{
	
	void add_focusable_view( const vfs::node* key, Pedestal::View* value );
	
	Pedestal::View* get_focusable_view( const vfs::node* key );
	
	void remove_focusable_view( const vfs::node* key );
	
}

#endif

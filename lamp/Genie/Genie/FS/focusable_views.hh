/*
	focusable_views.hh
	------------------
*/

#ifndef GENIE_FS_FOCUSABLEVIEWS_HH
#define GENIE_FS_FOCUSABLEVIEWS_HH

// Genie
#include "Genie/FS/FSTree_fwd.hh"


namespace Pedestal
{
	
	class View;
	
}

namespace Genie
{
	
	void add_focusable_view( const FSTree* key, Pedestal::View* value );
	
	Pedestal::View* get_focusable_view( const FSTree* key );
	
	void remove_focusable_view( const FSTree* key );
	
}

#endif


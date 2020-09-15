/*
	mask.hh
	-------
*/

#ifndef ICON_MASK_HH
#define ICON_MASK_HH

// icon-utils
#include "icon/icon.hh"


namespace icon
{
	
	union icon32_data;
	
	bool derive_mask( const icon32_data& icon, icon32_data& mask );
	
}

#endif

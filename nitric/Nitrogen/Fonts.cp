// Fonts.cp

#ifndef NITROGEN_FONTS_H
#include "Nitrogen/Fonts.h"
#endif

namespace Nitrogen {
	
	FontID GetFNum( ConstStr255Param name )
	{
		short fontNum;
		::GetFNum( name, &fontNum );
		return FontID( fontNum );
	}
	
	FontID GetFNum( const std::string& name )
	{
		return GetFNum( Str255( name ) );
	}
	
}


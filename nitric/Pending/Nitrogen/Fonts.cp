// Fonts.cp

#ifndef NITROGEN_FONTS_H
#include "Nitrogen/Fonts.h"
#endif

namespace Nitrogen {
	
	FontNumber GetFNum( ConstStr255Param name )
	{
		short fontNum;
		::GetFNum( name, &fontNum );
		return fontNum;
	}
	
	FontNumber GetFNum( const std::string& name )
	{
		return GetFNum( Str255( name ) );
	}
	
}


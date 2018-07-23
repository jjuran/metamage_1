/*
	TextEdit.cc
	-----------
*/

#include "TextEdit.hh"

// Mac OS
#ifndef __TEXTEDIT__
#include <TextEdit.h>
#endif


pascal void TEInit_patch()
{
}

pascal void TETextBox_patch( const char* p, long n, const Rect* r, short just )
{
	FontInfo fontInfo;
	GetFontInfo( &fontInfo );
	
	short h = r->left;
	short v = r->top + fontInfo.ascent;
	
	// Treat teFlushDefault like teFlushLeft.
	
	if ( just & 1 )
	{
		// center- or right-justified
		
		short rectWidth = r->right - r->left;
		short textWidth = TextWidth( p, 0, n );
		
		short margin = rectWidth - textWidth;
		
		switch ( just )
		{
			case teCenter:
				margin /= 2;
				// fall through
			
			case teFlushRight:
				h += margin;
				break;
			
			default:
				// Invalid parameter
				break;
		}
	}
	
	MoveTo( h, v );
	
	DrawText( p, 0, n );
}

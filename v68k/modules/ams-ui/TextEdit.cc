/*
	TextEdit.cc
	-----------
*/

#include "TextEdit.hh"

// Mac OS
#ifndef __TEXTEDIT__
#include <TextEdit.h>
#endif

// gear
#include "gear/find.hh"


static
void draw_text_line( const char*  p,
                     long         n,
                     short        h,
                     short        v,
                     short        rectWidth,
                     short        just );

pascal void TEInit_patch()
{
}

pascal void TETextBox_patch( const char* p, long n, const Rect* r, short just )
{
	FontInfo fontInfo;
	GetFontInfo( &fontInfo );
	
	const short line_height = fontInfo.ascent
	                        + fontInfo.descent
	                        + fontInfo.leading;
	
	short h = r->left;
	short v = r->top + fontInfo.ascent;
	
	const short rectWidth = r->right - r->left;
	
	const char* end = p + n;
	
	while ( p < end )
	{
		const char* q = gear::find_first_match( p, end, '\r', end );
		
		const char* first_space = gear::find_first_match( p, q, ' ', q );
		
		while ( q > first_space  &&  TextWidth( p, 0, q - p ) > rectWidth )
		{
			while ( *--q != ' ' ) continue;
		}
		
		if ( q == first_space )
		{
			while ( q > p + 1  &&  TextWidth( p, 0, q - p ) > rectWidth )
			{
				--q;
			}
		}
		
		draw_text_line( p, q - p, h, v, rectWidth, just );
		
		v += line_height;
		
		p = q + (q >= first_space);
	}
}

static
void draw_text_line( const char*  p,
                     long         n,
                     short        h,
                     short        v,
                     short        rectWidth,
                     short        just )
{
	// Treat teFlushDefault like teFlushLeft.
	
	if ( just & 1 )
	{
		// center- or right-justified
		
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

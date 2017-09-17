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

// ams-common
#include "QDGlobals.hh"
#include "raster_lock.hh"

// ams-ui
#include "scoped_port.hh"


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

static
void draw_text( const TERec& te )
{
	raster_lock lock;
	
	const Rect& destRect = te.destRect;
	
	EraseRect( &destRect );
	
	const short v = destRect.top + te.fontAscent;
	const short h = destRect.left + 1;
	
	const short rectWidth = destRect.right - destRect.left;
	
	draw_text_line( *te.hText, te.teLength, h, v, rectWidth, te.just );
}

pascal void TEInit_patch()
{
}

pascal TERec** TENew_patch( const Rect* destRect, const Rect* viewRect )
{
	TEHandle hTE = (TEHandle) NewHandleClear( sizeof (TERec) );
	
	if ( hTE )
	{
		Handle hText = NewHandle( 32 );
		
		if ( hText == NULL )
		{
			DisposeHandle( (Handle) hTE );
			
			return NULL;
		}
		
		hTE[0]->hText = hText;
	}
	
	GrafPtr thePort = *get_addrof_thePort();
	
	FontInfo fontInfo;
	GetFontInfo( &fontInfo );
	
	const short line_height = fontInfo.ascent
	                        + fontInfo.descent
	                        + fontInfo.leading;
	
	TERec& te = **hTE;
	
	te.destRect = *destRect;
	te.viewRect = *viewRect;
	
	te.lineHeight = line_height;
	te.fontAscent = fontInfo.ascent;
	
	te.txFont = thePort->txFont;
	te.txFace = thePort->txFace;
	te.txMode = thePort->txMode;
	te.txSize = thePort->txSize;
	te.inPort = thePort;
	
	return hTE;
}

pascal void TEDispose_patch( TERec** hTE )
{
	DisposeHandle( hTE[0]->hText );
	
	DisposeHandle( (Handle) hTE );
}

pascal void TESetText_patch( const char* p, long n, TERec** hTE )
{
	PtrToXHand( p, hTE[0]->hText, n );
	
	hTE[0]->selStart = n;
	hTE[0]->selEnd   = n;
	hTE[0]->teLength = n;
}

pascal void TEIdle_patch( TEHandle hTE )
{
}

pascal void TESetSelect_patch( long selStart, long selEnd, TEHandle hTE )
{
	TERec& te = **hTE;
	
	if ( selEnd > te.teLength )
	{
		selEnd = te.teLength;
	}
	
	te.selStart = selStart;
	te.selEnd   = selEnd;
}

pascal void TEActivate_patch( TERec** hTE )
{
	hTE[0]->active = true;
}

pascal void TEDeactivate_patch( TERec** hTE )
{
	hTE[0]->active = false;
}

pascal void TEUpdate_patch( const Rect* updateRect, TERec** hTE )
{
	TERec& te = **hTE;
	
	scoped_port thePort = te.inPort;
	
	draw_text( te );
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

/*
	TextEdit.cc
	-----------
*/

#include "TextEdit.hh"

// Mac OS
#ifndef __EVENTS__
#include <Events.h>
#endif
#ifndef __TEXTEDIT__
#include <TextEdit.h>
#endif
#ifndef __TEXTUTILS__
#include <TextUtils.h>
#endif

// gear
#include "gear/find.hh"

// ams-common
#include "callouts.hh"
#include "QDGlobals.hh"
#include "raster_lock.hh"

// ams-ui
#include "scoped_port.hh"
#include "utility_region.hh"


long Ticks     : 0x016A;
long CaretTime : 0x02F4;

short  TEScrpLength : 0x0AB0;
Handle TEScrpHandle : 0x0AB4;


class scoped_TERec
{
	private:
		GrafPtr     its_port;
		scoped_port its_port_scope;
		RgnHandle   its_saved_clipRgn;
		
		short its_font;
		short its_face;
		short its_mode;
		short its_size;
		
		// non-copyable
		scoped_TERec           ( const scoped_TERec& );
		scoped_TERec& operator=( const scoped_TERec& );
	
	public:
		scoped_TERec( const TERec& te );
		
		~scoped_TERec();
};

scoped_TERec::scoped_TERec( const TERec& te )
:
	its_port( te.inPort ),
	its_port_scope( its_port )
{
	fast_memcpy( &its_font,         &its_port->txFont, sizeof (short) * 4 );
	fast_memcpy( &its_port->txFont, &te.txFont,        sizeof (short) * 4 );
	
	its_saved_clipRgn = its_port->clipRgn;
	its_port->clipRgn = rectangular_utility_region( te.viewRect );
}

scoped_TERec::~scoped_TERec()
{
	its_port->clipRgn = its_saved_clipRgn;
	
	fast_memcpy( &its_port->txFont, &its_font, sizeof (short) * 4 );
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

static
void draw_text( const TERec& te )
{
	const Rect& destRect = te.destRect;
	const Rect& viewRect = te.viewRect;
	
	const short lineHeight = te.lineHeight;
	
	if ( te.active )
	{
		EraseRect( &viewRect );
	}
	
	const short* starts = te.lineStarts;
	
	const short viewHeight = viewRect.bottom - viewRect.top;
	
	const short scroll_delta = viewRect.top - destRect.top;
	
	const short skipped_lines = scroll_delta ? scroll_delta / lineHeight : 0;
	
	if ( skipped_lines >= te.nLines )
	{
		return;  // Somehow we're scrolled past the entire text.
	}
	
	/*
		"above" here means above viewRect.bottom -- so, all the lines either
		preceding the viewRect entirely or are at least partially visible.
		
		"first" is the same, except capped at nLines.  Subtracting the number
		of skipped lines gives us the number of lines we'll actually draw.
	*/
	
	const short above_lines = (scroll_delta + viewHeight - 1) / lineHeight + 1;
	const short first_lines = above_lines < te.nLines ? above_lines : te.nLines;
	const short drawn_lines = first_lines - skipped_lines;
	
	short v = destRect.top + te.fontAscent;
	short h = destRect.left + 1;
	
	starts += skipped_lines;
	v      += skipped_lines * lineHeight;
	
	const short rectWidth = destRect.right - destRect.left;
	
	short start = *starts++;
	
	const char* p = *te.hText + start;
	const short n = drawn_lines;
	
	for ( short i = 0;  i < n;  ++i )
	{
		const short next = *starts++;
		
		const short len = next - start;
		
		draw_text_line( p, len, h, v, rectWidth, te.just );
		
		p += len;
		v += lineHeight;
		
		start = next;
	}
}

static
void update_selRect( TERec& te )
{
	const char* pText = *te.hText;
	const short start = te.selStart;
	const short end   = te.selEnd;
	const short count = end - start;
	
	const short* starts = te.lineStarts + 1;
	
	short nth_line = -1;
	
	while ( ++nth_line, *starts++ < start )
	{
		continue;
	}
	
	if ( *--starts == start )
	{
		if ( start < te.teLength  ||  pText[ start - 1 ] == '\r' )
		{
			++nth_line;
			++starts;
		}
	}
	
	const short voffset = nth_line * te.lineHeight;
	
	te.selRect.top    = te.destRect.top + voffset;
	te.selRect.bottom = te.destRect.top + voffset + te.lineHeight;
	
	const short nextStart = *starts;
	const short thisStart = *--starts;
	const short leftChars = start - thisStart;
	
	const short left = te.destRect.left + TextWidth( pText, thisStart, leftChars );
	
	const bool bleedRight = count > 0  &&  end == nextStart;
	
	te.selRect.left  = left + (start > 0  &&  count > 0);
	te.selRect.right = bleedRight ? te.destRect.right
	                              : left + TextWidth( pText, start, count ) + 1;
}

static
void toggle_selRect( TERec& te )
{
	te.caretTime  = Ticks + CaretTime;
	te.caretState = ! te.caretState;
	
	InvertRect( &te.selRect );
}

static
void showhide_selection( TERec& te, bool show )
{
	if ( ! te.active )
	{
		return;
	}
	
	if ( !! te.caretState == show )
	{
		// Caret is already in desired state.
		return;
	}
	
	toggle_selRect( te );
}

static inline
void show_selection( TERec& te )
{
	showhide_selection( te, true );
}

static inline
void hide_selection( TERec& te )
{
	showhide_selection( te, false );
}

static inline
bool blinks( const TERec& te )
{
	return te.active  &&  te.selStart == te.selEnd;
}

pascal void TEInit_patch()
{
	CaretTime = 30;
	
//	TEScrpLength = 0;
	TEScrpHandle = NewHandle( 0 );
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
	if ( Handle h = hTE[0]->hText )
	{
		DisposeHandle( h );
	}
	
	DisposeHandle( (Handle) hTE );
}

pascal void TESetText_patch( const char* p, long n, TERec** hTE )
{
	TERec& te = **hTE;
	
	PtrToXHand( p, te.hText, n );
	
	te.selStart = n;
	te.selEnd   = n;
	te.teLength = n;
	
	TECalText( hTE );
	
	update_selRect( te );
}

pascal void TEIdle_patch( TEHandle hTE )
{
	TERec& te = **hTE;
	
	if ( blinks( te )  &&  Ticks >= te.caretTime )
	{
		scoped_TERec scope = te;
		
		toggle_selRect( te );
	}
}

static
short hit_test( const TERec& te, Point pt )
{
	short v = pt.v;
	short h = pt.h;
	
	v -= te.destRect.top;
	h -= te.destRect.left;
	
	const short line = v / te.lineHeight;
	
	if ( line < 0 )
	{
		return 0;
	}
	
	if ( line >= te.nLines )
	{
		return te.teLength;
	}
	
	const short start = te.lineStarts[ line ];
	
	const char* p = *te.hText;
	
	const short next = te.lineStarts[ line + 1 ];
	
	short hit = start;
	
	while ( hit < next )
	{
		const short width = TextWidth( p, hit, 1 );
		
		if ( (h -= width) < 0 )
		{
			const short half = width / 2u;  // Make sure half is signed
			
			return hit + (h + half > 0);
		}
		
		++hit;
	}
	
	return hit;
}

static
void update_selection( TERec& te, short selStart, short selEnd )
{
	if ( selStart == te.selStart  &&  selEnd == te.selEnd )
	{
		return;
	}
	
	raster_lock lock;
	
	hide_selection( te );
	
	te.selStart = selStart;
	te.selEnd   = selEnd;
	
	update_selRect( te );
	show_selection( te );
}

static
void autoscroll( TERec& te )
{
	short dv;
	
	if ( (dv = te.viewRect.top - te.selRect.top) > 0 )
	{
		// scroll up
	}
	else if ( (dv = te.viewRect.bottom - te.selRect.bottom) < 0 )
	{
		// scroll down
	}
	else
	{
		return;
	}
	
	OffsetRect( &te.destRect, 0, dv );
}

pascal void TEClick_patch( Point pt, char extend, TERec** hTE )
{
	TERec& te = **hTE;
	
	scoped_TERec scope = te;
	
	const short hit = hit_test( te, pt );
	
	te.selPoint  = pt;
	te.clickTime = Ticks;
	
	short start, end;
	
	if ( extend )
	{
		const short loc = te.clickLoc;
		
		start = hit < loc ? hit : loc;
		end   = hit ^ loc ^ start;
	}
	else
	{
		te.clickLoc = hit;
		
		start = end = hit;
	}
	
	update_selection( te, start, end );
	
	const short loc = te.clickLoc;
	
	while ( WaitMouseUp() )
	{
		GetMouse( &pt );
		
		const short hit = hit_test( te, pt );
		
		start = hit < loc ? hit : loc;
		end   = hit ^ loc ^ start;
		
		update_selection( te, start, end );
	}
}

pascal void TESetSelect_patch( long selStart, long selEnd, TEHandle hTE )
{
	TERec& te = **hTE;
	
	scoped_TERec scope = te;
	
	if ( selEnd > te.teLength )
	{
		selEnd = te.teLength;
	}
	
	if ( selStart > selEnd )
	{
		selStart = selEnd;
	}
	
	update_selection( te, selStart, selEnd );
}

pascal void TEActivate_patch( TERec** hTE )
{
	TERec& te = **hTE;
	
	scoped_TERec scope = te;
	
	te.active = true;
	
	update_selRect( te );
	show_selection( te );
}

pascal void TEDeactivate_patch( TERec** hTE )
{
	TERec& te = **hTE;
	
	scoped_port thePort = te.inPort;
	
	hide_selection( te );
	
	te.active = false;
}

static inline
bool has_selection( const TERec& te )
{
	return te.selStart < te.selEnd;
}

static inline
bool at_beginning( const TERec& te )
{
	return te.selEnd == 0;
}

static
void select( TERec& te, short pos )
{
	te.clickLoc = pos;
	te.selStart = pos;
	te.selEnd   = pos;
}

static
void delete_chars( TERec& te, short start, short end )
{
	const Ptr pText = *te.hText;
	
	const short len = end - start;
	
	BlockMoveData( pText + end, pText + start, te.teLength - end );
	
	te.clickLoc  = start;
	te.teLength -= len;
	te.selEnd    = start;
}

static inline
void delete_selection( TERec& te )
{
	delete_chars( te, te.selStart, te.selEnd );
}

static inline
void delete_prev_char( TERec& te )
{
	short end   =   te.selStart;
	short start = --te.selStart;
	
	delete_chars( te, start, end );
}

static inline
Size expanded_size( Size size )
{
	size *= 2;
	
	return size < 32 ? 32 : size;
}

static
void insert_char( TEHandle hTE, char c )
{
	TERec& te = **hTE;
	
	const Handle hText = te.hText;
	
	const Size size = GetHandleSize( hText );
	
	if ( size <= te.teLength )
	{
		SetHandleSize( hText, expanded_size( size ) );
	}
	
	short pos = te.selStart;
	short len = te.teLength;
	
	short n = len - pos;
	
	BlockMoveData( *hText + pos, *hText + pos + 1, n );
	
	hText[0][ pos ] = c;
	
	te.clickLoc = pos + 1;
	te.selStart = pos + 1;
	te.selEnd   = pos + 1;
	te.teLength = len + 1;
}

static
void insert_chars( TEHandle hTE, const char* p, long n )
{
	TERec& te = **hTE;
	
	const Handle hText = te.hText;
	
	Size size = GetHandleSize( hText );
	
	if ( size < te.teLength + n )
	{
		size *= 2;
		
		if ( size < te.teLength + n )
		{
			size = te.teLength + n;
		}
		
		SetHandleSize( hText, size );
	}
	
	short pos = te.selStart;
	short len = te.teLength;
	
	char* target = *hText + pos;
	
	BlockMoveData( target, target + n, len - pos );
	
	BlockMoveData( p, target, n );
	
	te.teLength = len + n;
	te.selStart = pos + n;
	te.selEnd   = pos + n;
}

pascal void TEKey_patch( short c, TERec** hTE )
{
	TERec& te = **hTE;
	
	scoped_TERec scope = te;
	
	raster_lock lock;
	
	hide_selection( te );
	
	switch ( c )
	{
		case kBackspaceCharCode:
			if ( has_selection( te ) )
			{
				delete_selection( te );
			}
			else if ( ! at_beginning( te ) )
			{
				delete_prev_char( te );
			}
			else
			{
				break;
			}
			
			TECalText( hTE );
			break;
		
		case kLeftArrowCharCode:
			/*
				It would be nice to just use selStart if there's a
				selection range, but that's not what TextEdit does, and
				applications should do the right thing and compensate
				for this on their own.
			*/
			
			select( te, te.selStart - (te.selStart != 0) );
			break;
			
		case kRightArrowCharCode:
			// Ditto above, for selEnd.
			
			select( te, te.selEnd + (te.selEnd != te.teLength) );
			break;
			
		case kUpArrowCharCode:
			select( te, 0 );
			break;
			
		case kDownArrowCharCode:
			select( te, te.teLength );
			break;
			
		
		default:
			if ( has_selection( te ) )
			{
				delete_selection( te );
			}
			
			insert_char( hTE, c );
			
			TECalText( hTE );
			break;
	}
	
	update_selRect( te );
	autoscroll( te );
	
	draw_text( te );
	
	update_selRect( te );
	show_selection( te );
}

static
OSErr copy_text( TEHandle hTE )
{
	TERec& te = **hTE;
	
	const Size length = te.selEnd - te.selStart;
	
	if ( length <= 0 )
	{
		return paramErr;
	}
	
	OSErr err = PtrToXHand( *te.hText + te.selStart, TEScrpHandle, length );
	
	if ( err == noErr )
	{
		TEScrpLength = length;
	}
	
	return err;
}

pascal void TECut_patch( TERec** hTE )
{
	OSErr err = copy_text( hTE );
	
	if ( err == noErr )
	{
		TEDelete( hTE );
	}
}

pascal void TECopy_patch( TERec** hTE )
{
	copy_text( hTE );
}

pascal void TEPaste_patch( TERec** hTE )
{
	TERec& te = **hTE;
	
	const Size selLength = te.selEnd - te.selStart;
	
	Handle hText = te.hText;
	
	SetHandleSize( hText, te.teLength );  // Possibly shrink the handle to fit.
	
	Munger( hText, te.selStart, NULL, selLength, *TEScrpHandle, TEScrpLength );
	
	te.teLength  = GetHandleSize( hText );
	te.selStart += TEScrpLength;
	te.selEnd    = te.selStart;
	
	TECalText( hTE );
	
	scoped_TERec scope = te;
	
	raster_lock lock;
	
	hide_selection( te );
	
	draw_text( te );
	
	update_selRect( te );
	show_selection( te );
}

pascal void TEDelete_patch( TERec** hTE )
{
	TERec& te = **hTE;
	
	if ( has_selection( te ) )
	{
		delete_selection( te );
		
		TECalText( hTE );
		
		scoped_TERec scope = te;
		
		raster_lock lock;
		
		hide_selection( te );
		
		draw_text( te );
		
		update_selRect( te );
		show_selection( te );
	}
	
	/*
		Deja Vu dumps a TE record's entire hText to its save files, instead of
		just teLength bytes, foiling our amortized constant time optimization
		of character insertion.  Trim the handle here to appease it.
	*/
	
	SetHandleSize( te.hText, te.teLength );
}

pascal void TEInsert_patch( const char* text, long length, TERec** hTE )
{
	TERec& te = **hTE;
	
	scoped_TERec scope = te;
	
	raster_lock lock;
	
	hide_selection( te );
	
	insert_chars( hTE, text, length );
	
	TECalText( hTE );
	
	draw_text( te );
	
	update_selRect( te );
	show_selection( te );
	
	/*
		Deja Vu dumps a TE record's entire hText to its save files, instead of
		just teLength bytes, foiling our amortized constant time optimization
		of character insertion.  Trim the handle here to appease it.
	*/
	
	SetHandleSize( te.hText, te.teLength );
}

pascal void TESetJust_patch( short just, TERec** hTE )
{
	hTE[0]->just = just;
}

pascal void TEUpdate_patch( const Rect* updateRect, TERec** hTE )
{
	TERec& te = **hTE;
	
	scoped_TERec scope = te;
	
	raster_lock lock;
	
	hide_selection( te );
	
	draw_text( te );
	
	show_selection( te );
}

pascal void TETextBox_patch( const char* p, long n, const Rect* r, short just )
{
	TEHandle hTE = TENew( r, r );
	
	TERec& te = **hTE;
	
	te.just = just;
	
	TESetText( p, n, hTE );
	
	scoped_TERec scope = te;
	
	raster_lock lock;
	
	EraseRect( r );
	
	draw_text( te );
	
	TEDispose( hTE );
}

pascal void TEScroll_patch( short dh, short dv, TERec** hTE )
{
	TERec& te = **hTE;
	
	OffsetRect( &te.destRect, dh, dv );
	OffsetRect( &te.selRect,  dh, dv );
	
	static RgnHandle updateRgn = NewRgn();
	
	raster_lock lock;
	
	ScrollRect( &te.viewRect, dh, dv, updateRgn );
	
	TEUpdate( &updateRgn[0]->rgnBBox, hTE );
}

pascal void TECalText_patch( TERec** hTE )
{
	TERec& te = **hTE;
	
	const Rect& destRect = te.destRect;
	
	const short rectWidth = destRect.right - destRect.left;
	
	const char* begin = *te.hText;
	const char* end   = begin + te.teLength;
	
	te.nLines = 0;
	
	short* starts = te.lineStarts;
	
	const char* p = begin;
	
	while ( p < end )
	{
		++te.nLines;
		*starts++ = p - begin;
		
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
		
		p = q + (q >= first_space);
	}
	
	*starts++ = te.teLength;
}

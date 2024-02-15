/*
	Pictures.cc
	-----------
*/

#include "Pictures.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// mac-glue-utils
#include "mac_glue/Memory.hh"

// log-of-war
#include "logofwar/report.hh"
#include "logofwar/print.hh"

// ams-common
#include "callouts.hh"
#include "QDGlobals.hh"

// ams-qd
#include "Picture-bits.hh"
#include "Pictures-common.hh"


#pragma exceptions off


using logofwar::hex32_t;

typedef Byte Op;

pascal PicHandle OpenPicture_patch( const Rect* frame )
{
	GrafPort& port = **get_addrof_thePort();
	
	enum { initial_size = 12 };
	
	Handle h = NewHandle( initial_size );
	
	if ( h )
	{
		port.picSave = h;
		
		char* p = *h + sizeof (Picture);
		
		*p++ = 0x11;  // picVersion
		*p++ = 0x01;  // version 1
		
		HidePen();
	}
	
	return (PicHandle) h;
}

pascal void ClosePicture_patch()
{
	GrafPort& port = **get_addrof_thePort();
	
	if ( Handle h = port.picSave )
	{
		ShowPen();
		
		Size size = mac::glue::GetHandleSize_raw( h );
		
		SetHandleSize( h, size + 1 );
		h[0][ size++ ] = 0xFF;  // EndOfPicture
		
		PicHandle picture = (PicHandle) h;
		
		picture[0]->picSize  = size;
		picture[0]->picFrame = port.portRect;
		
		port.picSave = NULL;
	}
}

pascal void PicComment_patch( short kind, short dataSize, Handle data )
{
}

static inline
long read_long( const UInt8*& p )
{
	long result;
	
	fast_memcpy( &result, p, sizeof result );
	
	p += sizeof result;
	
	return result;
}

static Point curvature;

static inline
const UInt8* set_curvature( const UInt8* p )
{
	fast_memcpy( &curvature, p, sizeof curvature );
	
	return p + sizeof curvature;
}

static Point picture_origin;

static inline
const UInt8* origin( const UInt8* p )
{
	picture_origin.h += read_word( p );
	picture_origin.v += read_word( p );
	
	SetOrigin( picture_origin.h, picture_origin.v );
	
	return p;
}

static inline
const UInt8* pen_size( const UInt8* p )
{
	Point size;
	fast_memcpy( &size, p, sizeof size );
	
	PenSize( size.h, size.v );
	
	return p + sizeof size;
}

static
const UInt8* set_pattern( const UInt8* p, Pattern& pattern )
{
	short dy = picture_origin.v & 7u;
//	short dx = picture_origin.h & 7u;
	
	short yd = 8 - dy;
//	short xd = 8 - dx;
	
	fast_memcpy( &pattern.pat[  0 ], p + yd, dy );
	fast_memcpy( &pattern.pat[ dy ], p +  0, yd );
	
	if ( short dx = picture_origin.h & 7u )
	{
		short xd = 8 - dx;
		
		for ( int i = 0;  i < 8;  ++i )
		{
			pattern.pat[ i ] = pattern.pat[ i ] >> dx
			                 | pattern.pat[ i ] << xd;
		}
	}
	
	return p + sizeof (Pattern);
}

static
const UInt8* text_ratio( const UInt8* p )
{
	const short v_numer = read_word( p );
	const short h_numer = read_word( p );
	
	const short v_denom = read_word( p );
	const short h_denom = read_word( p );
	
	return p;
}

static
const UInt8* line( const UInt8* p )
{
	const short v0 = read_word( p );
	const short h0 = read_word( p );
	
	const short v1 = read_word( p );
	const short h1 = read_word( p );
	
	GrafPort& port = **get_addrof_thePort();
	
	Point pt = port.pnLoc;
	
	MoveTo( h0, v0 );
	LineTo( h1, v1 );
	
	port.pnLoc = pt;
	
	return p;
}

static
const UInt8* short_line( const UInt8* p )
{
	const short v = read_word( p );
	const short h = read_word( p );
	
	const SInt8 dh = *p++;
	const SInt8 dv = *p++;
	
	GrafPort& port = **get_addrof_thePort();
	
	Point pt = port.pnLoc;
	
	MoveTo( h, v );
	
	Line( dh, dv );
	
	port.pnLoc = pt;
	
	return p;
}

static inline
const UInt8* short_line_from( const UInt8* p )
{
	const SInt8 dh = *p++;
	const SInt8 dv = *p++;
	
	Line( dh, dv );
	
	return p;
}

static
const UInt8* long_text( const UInt8* p )
{
	const short v = read_word( p );
	const short h = read_word( p );
	
	MoveTo( h, v );
	
	DrawString( p );
	
	MoveTo( h, v );
	
	const UInt8 n = *p++;
	
	return p + n;
}

static
const UInt8* text( const UInt8* p )
{
	GrafPort& port = **get_addrof_thePort();
	
	Point pt = port.pnLoc;
	
	DrawString( p );
	
	port.pnLoc = pt;
	
	const UInt8 n = *p++;
	
	return p + n;
}

static inline
const UInt8* text_dh( const UInt8* p )
{
	const UInt8 dh = *p++;
	
	Move( dh, 0 );
	
	return text( p );
}

static inline
const UInt8* text_dv( const UInt8* p )
{
	const UInt8 dv = *p++;
	
	Move( 0, dv );
	
	return text( p );
}

static inline
const UInt8* text_dhdv( const UInt8* p )
{
	const UInt8 dh = *p++;
	const UInt8 dv = *p++;
	
	Move( dh, dv );
	
	return text( p );
}

static Rect last_used_rect;

static
const UInt8* poly( const Byte* p, Op op )
{
	const UInt8* q = p;
	
	const short polySize = read_word( q );
	
	Handle h = NewHandle( polySize );
	
	BlockMoveData( p, *h, polySize );
	
	signed char verb = op - 0x70;
	
	StdPoly( verb, (PolyHandle) h );
	
	DisposeHandle( h );
	
	return p + polySize;
}

static
const Byte* do_opcode( const Byte* p )
{
	const Byte opcode = *p++;
	
	if ( (opcode - 0x30 & 0xC8) == 0x00 )
	{
		last_used_rect = read_Rect( p );
	}
	
	GrafPort& port = **get_addrof_thePort();
	
	switch ( opcode )
	{
		case 0x00:
		case 0x1E:  // DefHilite
			break;
		
		case 0x1A:  // RGBFgCol
		case 0x1F:  // OpColor
			p += 6;
			break;
		
		case 0xA0:
			p += 2;
			break;
		
		case 0xA1:  // long comment
			p += 2;
			// fall through
		case 0x2C:  // reserved
		case 0x2D:  // reserved
		case 0x2E:  // reserved
		case 0x2F:  // reserved
			p += read_word( p );
			break;
		
		case 0x01:
			p += read_word( p ) - 2;
			break;
		
		case 0x03:
			TextFont( read_word( p ) );
			break;
		
		case 0x04:
			TextFace( *p++ );
			break;
		
		case 0x05:
			TextMode( read_word( p ) );
			break;
		
		case 0x07:
			p = pen_size( p );
			break;
		
		case 0x08:
			PenMode( read_word( p ) );
			break;
		
		case 0x09:
			p = set_pattern( p, port.pnPat );
			break;
		
		case 0x0A:
			p = set_pattern( p, port.fillPat );
			break;
		
		case 0x0B:
			p = set_curvature( p );
			break;
		
		case 0x0C:
			p = origin( p );
			break;
		
		case 0x0D:
			TextSize( read_word( p ) );
			break;
		
		case 0x0E:
			ForeColor( read_long( p ) );
			break;
		
		case 0x10:
			p = text_ratio( p );
			break;
		
		case 0x20:
			p = line( p );
			break;
		
		case 0x22:
			p = short_line( p );
			break;
		
		case 0x23:
			p = short_line_from( p );
			break;
		
		case 0x28:
			p = long_text( p );
			break;
		
		case 0x29:
			p = text_dh( p );
			break;
		
		case 0x2A:
			p = text_dv( p );
			break;
		
		case 0x2B:
			p = text_dhdv( p );
			break;
		
		case 0x30:
		case 0x31:
		case 0x32:
		case 0x33:
		case 0x34:
		case 0x38:
		case 0x39:
		case 0x3A:
		case 0x3B:
		case 0x3C:
			StdRect( opcode & 0x7, &last_used_rect );
			break;
		
		case 0x40:
		case 0x41:
		case 0x42:
		case 0x43:
		case 0x44:
		case 0x48:
		case 0x49:
		case 0x4A:
		case 0x4B:
		case 0x4C:
			StdRRect( opcode & 0x7, &last_used_rect, curvature.h, curvature.v );
			break;
		
		case 0x50:
		case 0x51:
		case 0x52:
		case 0x53:
		case 0x54:
		case 0x58:
		case 0x59:
		case 0x5A:
		case 0x5B:
		case 0x5C:
			StdOval( opcode & 0x7, &last_used_rect );
			break;
		
		case 0x60:
		case 0x61:
		case 0x62:
		case 0x63:
		case 0x64:
		case 0x68:
		case 0x69:
		case 0x6A:
		case 0x6B:
		case 0x6C:
			short start, extent;
			start  = read_word( p );
			extent = read_word( p );
			
			StdArc( opcode & 0x7, &last_used_rect, start, extent );
			break;
		
		case 0x70:
		case 0x71:
		case 0x72:
		case 0x73:
		case 0x74:
			p = poly( p, opcode );
			break;
		
		case 0x90:
		case 0x98:
		case 0x99:
			p = draw_bits( p, opcode & 1 );
			break;
		
		case 0xFF:
			return NULL;
		
		default:
			ERROR = "Aborting picture at unknown opcode ", (hex32_t) opcode;
			return NULL;
	}
	
	return p;
}

static
const Byte* do_opcode2( const Byte* p )
{
	const UInt16* p2 = (const UInt16*) p;
	
	if ( *p++ == 0 )
	{
		p = do_opcode( p );
		
		if ( (long) p & 1 )
		{
			++p;
		}
		
		return p;
	}
	
	const UInt16 opcode = *p2++;
	
	switch ( opcode )
	{
		case 0x0C00:
			// HeaderOp
			p2 += 24 / 2u;
			break;
		
		default:
			return NULL;
	}
	
	return (const Byte*) p2;
}

pascal void KillPicture_patch( PicHandle picture )
{
	DisposeHandle( (Handle) picture );
}

pascal void DrawPicture_patch( PicHandle pic, const Rect* dstRect )
{
	GrafPort& port = **get_addrof_thePort();
	
	Size size = mac::glue::GetHandleSize_raw( (Handle) pic );
	
	if ( size < sizeof (Picture) + 4 )
	{
		return;  // $1101FF is valid, but empty
	}
	
	const short txFont = port.txFont;
	const short txFace = port.txFace;
	const short txMode = port.txMode;
	const short txSize = port.txSize;
	
	const UInt8* end = (UInt8*) pic[0] + size;
	const UInt8* p   = (UInt8*) (pic[0] + 1);
	
	PenState penState;
	GetPenState( &penState );
	
	port.pnMode = patCopy;
	
	UInt8 c = *p++;
	
	if ( c == 0 )
	{
		c = *p++;
	}
	
	if ( c != 0x11 )
	{
		return;
	}
	
	const Rect& frame = pic[0]->picFrame;
	
	const short dh = dstRect->left - frame.left;
	const short dv = dstRect->top  - frame.top;
	
	const Point saved_origin = (const Point&) port.portRect;
	
	SetOrigin( saved_origin.h - dh, saved_origin.v - dv );
	
	picture_origin = (const Point&) port.portRect;
	
	const long clipTopLeft = (long&) port.clipRgn[0]->rgnBBox;
	
	OffsetRgn( port.clipRgn, -dh, -dv );
	
	uint8_t version = *p++;
	
	if ( version == 1 )
	{
		while ( p  &&  p < end )
		{
			p = do_opcode( p );
		}
	}
	else if ( version == 2 )
	{
		++p;  // skip $FF after $02
		
		while ( p  &&  p < end )
		{
			p = do_opcode2( p );
		}
	}
	else
	{
		WARNING = "Unknown PICT format ", version;
	}
	
	if ( clipTopLeft != (long&) port.clipRgn[0]->rgnBBox )
	{
		/*
			Only offset the clipRgn back if it moved in the first place.
			It is of course possible that both dh and dv are zero, but the
			real concern is a clip region that approaches the edges of
			QuickDraw coordinate space on only some sides.  For example:
			
				clip region: {-32767,-32767 - 512,342}
			
			It's admittedly far-fetched, but it's totally possible.  Drawing
			a picture offset from the port's top left would unsuccessfully try
			to offset the clip region up and/or left, but the reverse offset
			would succeed!  Therefore we check to see if the initial offset
			occurred to determine if it needs to be reversed.
		*/
		
		OffsetRgn( port.clipRgn, dh, dv );
	}
	
	SetOrigin( saved_origin.h, saved_origin.v );
	
	SetPenState( &penState );
	
	TextFont( txFont );
	TextFace( txFace );
	TextMode( txMode );
	TextSize( txSize );
}

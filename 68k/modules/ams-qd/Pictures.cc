/*
	Pictures.cc
	-----------
*/

#include "Pictures.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// log-of-war
#include "logofwar/report.hh"
#include "logofwar/print.hh"

// quickdraw
#include "qd/pack_bits.hh"

// ams-common
#include "callouts.hh"
#include "QDGlobals.hh"


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
		
		Size size = GetHandleSize( h );
		
		SetHandleSize( h, size + 1 );
		h[0][ size++ ] = 0xFF;  // EndOfPicture
		
		PicHandle picture = (PicHandle) h;
		
		picture[0]->picSize  = size;
		picture[0]->picFrame = port.portRect;
		
		port.picSave = NULL;
	}
}

static
short read_word( const UInt8*& p )
{
	short result;
	fast_memcpy( &result, p, sizeof result );
	
	p += sizeof result;
	
	return result;
}

static
long read_long( const UInt8*& p )
{
	long result;
	
	fast_memcpy( &result, p, sizeof result );
	
	p += sizeof result;
	
	return result;
}

static
Rect read_Rect( const UInt8*& p )
{
	Rect result;
	fast_memcpy( &result, p, sizeof result );
	
	p += sizeof result;
	
	return result;
}

static Point curvature;

static
const UInt8* set_curvature( const UInt8* p )
{
	fast_memcpy( &curvature, p, sizeof curvature );
	
	return p + sizeof curvature;
}

static
const UInt8* pen_size( const UInt8* p )
{
	Point size;
	fast_memcpy( &size, p, sizeof size );
	
	PenSize( size.h, size.v );
	
	return p + sizeof size;
}

static
const UInt8* pen_pat( const UInt8* p )
{
	Pattern pattern;
	fast_memcpy( &pattern, p, sizeof pattern );
	
	PenPat( &pattern );
	
	return p + 8;
}

static
const UInt8* fill_pat( const UInt8* p )
{
	GrafPort& port = **get_addrof_thePort();
	
	fast_memcpy( &port.fillPat, p, sizeof (Pattern) );
	
	return p + sizeof (Pattern);
}

static
const UInt8* line( const UInt8* p )
{
	const short v0 = read_word( p );
	const short h0 = read_word( p );
	
	const short v1 = read_word( p );
	const short h1 = read_word( p );
	
	MoveTo( h0, v0 );
	LineTo( h1, v1 );
	
	return p;
}

static
const UInt8* short_line( const UInt8* p )
{
	const short v = read_word( p );
	const short h = read_word( p );
	
	const SInt8 dh = *p++;
	const SInt8 dv = *p++;
	
	MoveTo( h, v );
	
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
const UInt8* text_dh( const UInt8* p )
{
	const UInt8 dh = *p++;
	
	Move( dh, 0 );
	
	GrafPort& port = **get_addrof_thePort();
	
	Point pt = port.pnLoc;
	
	DrawString( p );
	
	port.pnLoc = pt;
	
	const UInt8 n = *p++;
	
	return p + n;
}

static
const UInt8* text_dv( const UInt8* p )
{
	const UInt8 dv = *p++;
	
	Move( 0, dv );
	
	GrafPort& port = **get_addrof_thePort();
	
	Point pt = port.pnLoc;
	
	DrawString( p );
	
	port.pnLoc = pt;
	
	const UInt8 n = *p++;
	
	return p + n;
}

static
const UInt8* text_dhdv( const UInt8* p )
{
	const UInt8 dh = *p++;
	const UInt8 dv = *p++;
	
	Move( dh, dv );
	
	GrafPort& port = **get_addrof_thePort();
	
	Point pt = port.pnLoc;
	
	DrawString( p );
	
	port.pnLoc = pt;
	
	const UInt8 n = *p++;
	
	return p + n;
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
const UInt8* draw_bits( const UInt8* p )
{
	BitMap bitmap;
	
	short rowBytes = read_word( p );
	
	bitmap.bounds = read_Rect( p );
	
	if ( rowBytes < 0 )
	{
		rowBytes &= 0x3FFF;
		
		p += 60;  // skip PixMap guts
	}
	
	bitmap.rowBytes = rowBytes;
	
	Rect srcRect = read_Rect( p );
	Rect dstRect = read_Rect( p );
	
	const short mode = read_word( p );
	
	short n_rows = bitmap.bounds.bottom - bitmap.bounds.top;
	
	if ( rowBytes < 8 )
	{
		bitmap.baseAddr = (Ptr) p;
		
		p += rowBytes * n_rows;
		
		StdBits( &bitmap, &srcRect, &dstRect, mode, NULL );
		
		return p;
	}
	
	bitmap.bounds.top    = 0;
	bitmap.bounds.bottom = 1;
	
	srcRect.top    = 0;
	srcRect.bottom = 1;
	
	dstRect.bottom = dstRect.top;
	
	if ( rowBytes <= 250 )
	{
		UInt8 buffer[ 256 ];
		
		bitmap.baseAddr = (Ptr) buffer;
		
		do
		{
			UInt8 count = *p++;
			
			UInt8* dst = buffer;
			
			quickdraw::unpack_bits( p, dst, rowBytes );
			
			++dstRect.bottom;
			
			StdBits( &bitmap, &srcRect, &dstRect, mode, NULL );
			
			++dstRect.top;
		}
		while ( --n_rows > 0 );
	}
	
	return p;
}

static
const Byte* do_opcode( const Byte* p )
{
	const Byte opcode = *p++;
	
	if ( (opcode - 0x30 & 0xC8) == 0x00 )
	{
		last_used_rect = read_Rect( p );
	}
	
	switch ( opcode )
	{
		case 0x00:
		case 0x1E:  // DefHilite
			break;
		
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
		
		case 0x09:
			p = pen_pat( p );
			break;
		
		case 0x0A:
			p = fill_pat( p );
			break;
		
		case 0x0B:
			p = set_curvature( p );
			break;
		
		case 0x0D:
			TextSize( read_word( p ) );
			break;
		
		case 0x0E:
			ForeColor( read_long( p ) );
			break;
		
		case 0x20:
			p = line( p );
			break;
		
		case 0x22:
			p = short_line( p );
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
			p = draw_bits( p );
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
	
	Size size = GetHandleSize( (Handle) pic );
	
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

/*
	Pictures.cc
	-----------
*/

#include "Pictures.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// quickdraw
#include "qd/pack_bits.hh"

// ams-common
#include "callouts.hh"
#include "QDGlobals.hh"


union Word
{
	SInt16 s16;
	UInt8  u8[2];
};

static
short read_word( const UInt8*& p )
{
	Word result;
	
	result.u8[ 0 ] = *p++;
	result.u8[ 1 ] = *p++;
	
	return result.s16;
}

static
Rect read_Rect( const UInt8*& p )
{
	Rect result;
	
	result.top    = read_word( p );
	result.left   = read_word( p );
	result.bottom = read_word( p );
	result.right  = read_word( p );
	
	return result;
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
const UInt8* short_line( const UInt8* p, const Rect& target, const Rect& frame )
{
	const short v = read_word( p );
	const short h = read_word( p );
	
	const SInt8 dh = *p++;
	const SInt8 dv = *p++;
	
	MoveTo( h + target.left - frame.left, v + target.top - frame.top );
	
	Line( dh, dv );
	
	return p;
}

static
const UInt8* draw_bits( const UInt8* p, const Rect& target, const Rect& frame )
{
	BitMap bitmap;
	
	const short rowBytes = read_word( p );
	
	bitmap.rowBytes = rowBytes;
	
	bitmap.bounds = read_Rect( p );
	
	Rect srcRect = read_Rect( p );
	Rect dstRect = read_Rect( p );
	
	const short mode = read_word( p );
	
	OffsetRect( &dstRect, target.left - frame.left, target.top - frame.top );
	
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

pascal void DrawPicture_patch( PicHandle pic, const Rect* dstRect )
{
	GrafPort& port = **get_addrof_thePort();
	
	Size size = GetHandleSize( (Handle) pic );
	
	const UInt8* end = (UInt8*) pic[0] + size;
	const UInt8* p   = (UInt8*) (pic[0] + 1);
	
	PenState penState;
	GetPenState( &penState );
	
	port.pnMode = patCopy;
	
	while ( p < end )
	{
		UInt8 c = *p++;
		
		switch ( c )
		{
			case 0x11:
				++p;
				continue;
			
			case 0xA0:
				p += 2;
				continue;
			
			case 0x01:
				p += read_word( p ) - 2;
				continue;
			
			case 0x09:
				p = pen_pat( p );
				continue;
			
			case 0x22:
				p = short_line( p, *dstRect, pic[0]->picFrame );
				continue;
			
			case 0x90:
			case 0x98:
				p = draw_bits( p, *dstRect, pic[0]->picFrame );
				continue;
			
			case 0xFF:
			default:
				break;
		}
		
		break;
	}
	
	SetPenState( &penState );
}

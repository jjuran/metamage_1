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
#include "QDGlobals.hh"
#include "redraw_lock.hh"


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
const UInt8* draw_bits( const UInt8* p, const Rect& target, const Rect& frame )
{
	BitMap bitmap;
	
	const short rowBytes = read_word( p );
	
	bitmap.rowBytes = rowBytes;
	
	bitmap.bounds = read_Rect( p );
	
	Rect srcRect = read_Rect( p );
	Rect dstRect = read_Rect( p );
	
	const short mode = read_word( p );
	
	short n_rows = bitmap.bounds.bottom - bitmap.bounds.top;
	
	bitmap.bounds.top    = 0;
	bitmap.bounds.bottom = 1;
	
	srcRect.top    = 0;
	srcRect.bottom = 1;
	
	dstRect.bottom = dstRect.top;
	
	OffsetRect( &dstRect, target.left - frame.left, target.top - frame.top );
	
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
	
	redraw_lock lock( port.portBits.baseAddr );
	
	const UInt8* end = (UInt8*) pic[0] + pic[0]->picSize;
	const UInt8* p   = (UInt8*) (pic[0] + 1);
	
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
			
			case 0x98:
				p = draw_bits( p, *dstRect, pic[0]->picFrame );
				continue;
			
			case 0xFF:
			default:
				break;
		}
		
		break;
	}
}

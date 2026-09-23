/*
	bits.cc
	-------
	
	Bits and text fade-in demo for Advanced Mac Substitute
	
	Copyright 2026, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
*/

#include "bits.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// production-logo-app-utils
#include "wait.hh"

// BitsAndTextFadeInDemo
#include "patterns.hh"


#define COUNT(array)  (sizeof (array) / sizeof (array)[0])


enum
{
	lock_screen   = 0xFFFFFFEC,
	unlock_screen = 0xFFFFFFEA,
};

struct bits_header
{
	UInt16  magic;
	UInt16  stride;
	UInt16  height;
	UInt16  width;
};

static inline
void blit( const BitMap* src, const Rect* dstRect, short mode )
{
	StdBits( src, &src->bounds, dstRect, mode, NULL );
}

static
void make_bitmap( BitMap& bitmap, short bits_id )
{
	Handle h = GetResource( 'BITS', bits_id );
	
	HLock( h );
	
	const bits_header& header = *(const bits_header*) *h;
	
	bitmap.baseAddr = *h + sizeof header;
	bitmap.rowBytes = header.stride;
	
	*(UInt32*) &bitmap.bounds.top    = 0;
	*(UInt32*) &bitmap.bounds.bottom = *(const UInt32*) &header.height;
}

void fade_in_bits( short center_h, short bottom_v )
{
	BitMap face;
	BitMap mask;
	
	make_bitmap( face, 128 );
	make_bitmap( mask, 129 );
	
	const short bits_dh = face.bounds.right - face.bounds.left;
	const short bits_dv = face.bounds.bottom - face.bounds.top;
	
	const short bits_h = center_h - bits_dh / 2u;
	const short bits_v = bottom_v - bits_dv;
	
	Rect bits_rect =
	{
		bits_v,
		bits_h,
		bottom_v,
		bits_h + bits_dh,
	};
	
	PenMode( notPatOr );
	
	int p_len = COUNT( patterns );
	
	const Pattern* pat = patterns;
	
	for ( int i = 0;  i < p_len;  ++i, ++pat )
	{
		asm { JSR lock_screen };
		
		blit( &mask, &bits_rect, srcBic );
		blit( &face, &bits_rect, srcOr  );
		
		PenPat( pat );
		
		PaintRect( &bits_rect );
		
		asm { JSR unlock_screen };
		
		wait_or_exit( 2 );
	}
}

/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

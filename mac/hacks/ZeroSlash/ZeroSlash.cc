/*
	ZeroSlash.cc
	------------
	
	Zero Slash INIT for Advanced Mac Substitute
	
	Copyright 2026, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	This program patches the system font (Chicago 12),
	updating the zero glyph to include a slash, which
	makes it distinguishable from the uppercase 'O'.
	
	As written, this INIT wouldn't have any effect in
	Mac OS -- the modified resource would be lost when
	the INIT's resource file is closed.
	
*/

// Mac OS
#ifndef __FONTS__
#include <Fonts.h>
#endif
#ifndef __RESOURCES__
#include <Resources.h>
#endif
#ifndef __TEXTUTILS__
#include <TextUtils.h>
#endif


#define PACK16( _15, _14, _13,_12,  \
                _11, _10,  _9, _8,  \
                 _7,  _6,  _5, _4,  \
                 _3,  _2,  _1, _0 ) \
	(                            \
		+ (_15 << 15)  \
		| (_14 << 14)  \
		| (_13 << 13)  \
		| (_12 << 12)  \
		| (_11 << 11)  \
		| (_10 << 10)  \
		| ( _9 <<  9)  \
		| ( _8 <<  8)  \
		| ( _7 <<  7)  \
		| ( _6 <<  6)  \
		| ( _5 <<  5)  \
		| ( _4 <<  4)  \
		| ( _3 <<  3)  \
		| ( _2 <<  2)  \
		| ( _1 <<  1)  \
		| ( _0 <<  0)  \
	)

#define PACK6( _5, _4, _3, _2, _1, _0 )  \
	PACK16( _5, _4, _3, _2, _1, _0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 )

#define _ 0
#define X 1


enum
{
	zero_height = 9,
	zero_width  = 6,
};

static const UInt16 zero_data[] =
{
	PACK6( _,X,X,X,X,_ ),
	PACK6( X,X,_,_,X,X ),
	PACK6( X,X,_,_,X,X ),
	PACK6( X,X,_,X,X,X ),
	PACK6( X,X,X,X,X,X ),
	PACK6( X,X,X,_,X,X ),
	PACK6( X,X,_,_,X,X ),
	PACK6( X,X,_,_,X,X ),
	PACK6( _,X,X,X,X,_ ),
};

static const BitMap zero_bitmap =
{
	(Ptr) zero_data,
	2,
	{ 0, 0, zero_height, zero_width },
};

inline
asm
unsigned long mulu_w( unsigned short a : __D0,
                      unsigned short b : __D1 )
{
	MULU.W   D1,D0
}

static
void patch_glyph( short          firstChar,
                  short          missingChar,
                  const short*   locTable,
                  short          c,
                  const BitMap&  srcBits,
                  const BitMap&  dstBits,
                  short          top )
{
	c -= firstChar;
	
	if ( c > missingChar )
	{
		return;
	}
	
	const Rect& srcRect = srcBits.bounds;
	
	const short this_offset = locTable[ c ];
	const short next_offset = locTable[ c + 1 ];
	
	const short src_width = srcRect.right - srcRect.left;
	const short dst_width = next_offset   - this_offset;
	
	if ( src_width != dst_width )
	{
		return;
	}
	
	Rect dstRect;
	
	dstRect.top    = top;
	dstRect.bottom = top + srcRect.bottom;
	
	dstRect.left  = this_offset;
	dstRect.right = next_offset;
	
	CopyBits( &srcBits, &dstBits, &srcRect, &dstRect, srcCopy, NULL );
}

static inline
void install_Chicago_12_patch( Handle h )
{
	HNoPurge( h );
	
	QDGlobals qd;
	GrafPort  port;
	
	InitGraf( &qd.thePort );
	
	OpenPort( &port );
	
	const FontRec& rec = **(FontRec**) h;
	
	Ptr dst = (Ptr) &rec + sizeof (FontRec);
	
	const short fRectHeight = rec.fRectHeight;
	
	const short top = fRectHeight - rec.ascent;
	
	const short first   = rec.firstChar;
	const short missing = rec.lastChar + 1 - first;
	
	const short rowBytes = rec.rowWords * 2;
	
	BitMap dstBits = { dst, rowBytes, { 0, 0, fRectHeight, rowBytes * 8 } };
	
	const short* locTable = (short*) (dst + mulu_w( rowBytes, fRectHeight ));
	
	patch_glyph( first, missing, locTable, '0', zero_bitmap, dstBits, top );
	
	ClosePort( &port );
}

int main()
{
	if ( Handle h = GetResource( 'FONT', 12 ) )
	{
		install_Chicago_12_patch( h );
	}
	
	return 0;
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

/*
	Intro.cc
	--------
	
	Introduction painting generator for Advanced Mac Substitute
	
	Copyright 2025, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	This application generates a MacPaint document for use in
	the screencast that demonstrates MacPaint running within
	Advanced Mac Substitute.  It doesn't run as part of the
	screencast itself, but in advance, creating the document
	used in the screencast under the name "Introduction".
	
*/

// Mac OS
#ifndef __FONTS__
#include <Fonts.h>
#endif
#ifndef __ICONS__
#include <Icons.h>
#endif
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


#pragma exceptions off


short SFSaveDisk : 0x0214;


enum
{
	pntg_height = 72 * 10,
	pntg_width  = 72 *  8,
	
	pntg_stride = pntg_width / 8,
	pntg_size   = pntg_stride * pntg_height,
};

static GrafPort draw_port;

static
void draw()
{
	const short center_h = pntg_width  / 2u;
	const short center_v = pntg_height / 3u - 4;
	
	const short icon_dh = 64;
	const short icon_dv = 64;
	
	const short icon_h = center_h - icon_dh / 2u;
	const short icon_v = center_v - icon_dv - 24;
	
	const Rect icon_rect =
	{
		icon_v,
		icon_h,
		icon_v + icon_dv,
		icon_h + icon_dh,
	};
	
	const short micn = 42;
	
	PlotIconID( &icon_rect, 0, 0, micn );
	
	StringPtr s;
	
	short text_h;
	short text_v;
	
	TextFont(  0 );
	TextSize( 24 );
	
	s = "\p" "Advanced Mac Substitute";
	
	text_v = icon_rect.top - 36 - 34 * 2;
	
	text_h = center_h - StringWidth( s ) / 2u;
	
	MoveTo( text_h, text_v );
	
	DrawString( s );
	
	s = "\p" "is a reimplementation";
	
	text_v += 34;
	
	MoveTo( text_h, text_v );
	
	DrawString( s );
	
	s = "\p" "of 1980s-era Mac OS.";
	
	text_v += 34;
	
	MoveTo( text_h, text_v );
	
	DrawString( s );
	
	const short prose_h = text_h;
	
	TextFont(  3 );
	TextSize( 18 );
	
	s = "\p" "Advanced Mac Substitute";
	
	text_v = center_v;
	text_h = center_h - StringWidth( s ) / 2u;
	
	MoveTo( text_h, text_v );
	
	DrawString( s );
	
	s = "\p" "by Josh Juran";
	
	text_v += 36;
	
	text_h = center_h - StringWidth( s ) / 2u;
	
	MoveTo( text_h, text_v );
	
	DrawString( s );
	
	s = "\p" "https://www.v68k.org/ams/";
	
	text_v += 60;
	
	text_h = center_h - StringWidth( s ) / 2u;
	
	MoveTo( text_h, text_v );
	
	DrawString( s );
	
	TextFont(  0 );
	TextSize( 24 );
	
	text_h = prose_h;
	
	s = "\p" "Everything in this video";
	
	text_v += 72;
	
	MoveTo( text_h, text_v );
	
	DrawString( s );
	
	s = "\p" "was recorded running in";
	
	text_v += 34;
	
	MoveTo( text_h, text_v );
	
	DrawString( s );
	
	s = "\p" "Advanced Mac Substitute.";
	
	text_v += 34;
	
	MoveTo( text_h, text_v );
	
	DrawString( s );
	
	s = "\p" "The only Apple code used";
	
	text_v += 72;
	
	MoveTo( text_h, text_v );
	
	DrawString( s );
	
	s = "\p" "was MacPaint itself.";
	
	text_v += 34;
	
	MoveTo( text_h, text_v );
	
	DrawString( s );
	
	s = "\p" "The rest is open source.";
	
	text_v += 72;
	
	MoveTo( text_h, text_v );
	
	DrawString( s );
}

static
void create_port()
{
	OpenPort( &draw_port );
	
	Ptr baseAddr = NewPtr( pntg_size );
	
	BitMap bitmap = { baseAddr, pntg_stride, { 0, 0, pntg_height, pntg_width } };
	
	SetPortBits( &bitmap );
	
	draw_port.portRect = bitmap.bounds;
	
	ClipRect( &bitmap.bounds );
	
	CopyRgn( draw_port.clipRgn, draw_port.visRgn );
	
	EraseRect( &bitmap.bounds );
}

static
OSErr write_file()
{
	const Byte* filename = "\p" "Introduction to AMS.pntg";
	
	OSErr err;
	short refnum;
	
	err = Create( filename, -SFSaveDisk, 'MPNT', 'PNTG' );
	
	err = FSOpen( filename, fsRdWrPerm, &refnum );
	
	if ( err == noErr )
	{
		Byte empty_block[ 512 ] = {};
		
		Size size = sizeof empty_block;
		
		err = FSWrite( refnum, &size, &empty_block );
		
		Ptr p = draw_port.portBits.baseAddr;
		
		for ( int i = 0;  err == noErr  &&  i < pntg_height;  ++i )
		{
			char buffer[ pntg_stride + 1 ];
			
			Ptr dst = buffer;
			
			PackBits( &p, &dst, pntg_stride );
			
			size = dst - buffer;
			
			err = FSWrite( refnum, &size, &buffer );
		}
		
		FSClose( refnum );
	}
	
	return err;
}

int main()
{
	InitGraf( &qd.thePort );
	InitFonts();
	
	create_port();
	
	draw();
	
	write_file();
	
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

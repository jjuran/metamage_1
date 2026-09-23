/*
	text.cc
	-------
	
	Bits and text fade-in demo for Advanced Mac Substitute
	
	Copyright 2026, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
*/

#include "text.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
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

void inscribe( const Byte* s )
{
	GrafPort& port = *qd.thePort;
	
	short pen_h = port.pnLoc.h;
	
	Rect column;
	
	column.top    = port.pnLoc.v - 20;
	column.bottom = port.pnLoc.v + 10;
	
	const short advance_rate = 2;
	const short column_width = 2;
	const short string_width = StringWidth( s );
	
	TextMode( srcBic );
	
	PenMode( notPatOr );
	
	const int p_len = COUNT( patterns );
	
	const short end = (string_width + column_width) / advance_rate + p_len - 1;
	
	short frontier = pen_h;
	
	for ( int t = 0;  t < end;  ++t )
	{
		port.pnLoc.h = pen_h;
		
		frontier += advance_rate;
		
		column.left  = frontier;
		column.right = pen_h + string_width;
		
		asm { JSR lock_screen };
		
		DrawString( s );
		
		PenPat( &qd.white );
		
		PaintRect( &column );
		
		column.right = frontier;
		column.left  = frontier - column_width;
		
		for ( int i = 0;  i < p_len;  ++i )
		{
			PenPat( &patterns[ i ] );
			
			PaintRect( &column );
			
			column.left  -= column_width;
			column.right -= column_width;
			
			if ( column.left < pen_h )
			{
				break;
			}
		}
		
		asm { JSR unlock_screen };
		
		wait_or_exit( 0 );
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

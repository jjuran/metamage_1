/*
	SilhouetteFadeInDemo.cc
	-----------------------
	
	Silhouette fade-in demo for classic Mac OS
	
	Copyright 2026, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
*/

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// production-logo-app-utils
#include "init.hh"
#include "terminate.hh"
#include "wait.hh"
#include "window.hh"


#define COUNT(array)  (sizeof (array) / sizeof (array)[0])


static const Pattern patterns[] =
{
	{ 0x00, 0x11, 0x00, 0x44, 0x00, 0x11, 0x00, 0x44 },
	{ 0x00, 0x55, 0x00, 0x55, 0x00, 0x55, 0x00, 0x55 },
	{ 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55 },
	{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
};

int main()
{
	init();
	
	WindowRef window = create_window();
	
	ForeColor( whiteColor );
	BackColor( blackColor );
	
	if ( RgnHandle logo_region = (RgnHandle) GetResource( 'RGN ', 128 ) )
	{
		const Rect& bbox = logo_region[0]->rgnBBox;
		
		short rgn_width  = bbox.right - bbox.left;
		short rgn_height = bbox.bottom - bbox.top;
		
		const Rect& portRect = window->portRect;
		
		short window_width  = portRect.right - portRect.left;
		short window_height = portRect.bottom - portRect.top;
		
		short dh = (window_width  - rgn_width ) / 2u;
		short dv = (window_height - rgn_height) / 2u;
		
		OffsetRgn( logo_region, dh, dv );
		
		wait_or_exit( 15 );
		
		for ( int i = 0;  i < COUNT( patterns );  ++i )
		{
			wait_or_exit( 15 );
			
			PenPat( &patterns[ i ] );
			
			PaintRgn( logo_region );
		}
		
		wait_or_exit( 60 * 3 );
		
		EraseRect( &logo_region[0]->rgnBBox );
		
		wait_or_exit( 30 );
	}
	
	terminate( false );
	
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

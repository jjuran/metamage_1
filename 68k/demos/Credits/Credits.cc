/*
	Credits.cc
	----------
	
	Credits screen generator for Advanced Mac Substitute screencasts
	
	Copyright 2025, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	This application draws the credits screen used at the end of
	screencasts of Advanced Mac Substitute.  It doesn't run as
	part of the screencast itself, but in advance, so that the
	credits image can be extracted into a ShutdownScreen file.
	
*/

// Mac OS
#ifndef __ICONS__
#include <Icons.h>
#endif
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif


#define LOWMEM( addr, type )  (*(type*) (addr))

#define CrsrPin  LOWMEM( 0x0834, Rect )


static
void draw_window( WindowRef window )
{
	const Rect& portRect = window->portRect;
	
	PaintRect( &portRect );
	
	const short window_width  = portRect.right - portRect.left;
	const short window_height = portRect.bottom - portRect.top;
	
	const short center_h = window_width  / 2u;
	const short center_v = window_height / 2u;
	
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
	
	ForeColor( whiteColor );
	BackColor( blackColor );
	
	TextFont(  3 );
	TextSize( 18 );
	
	StringPtr s;
	
	short text_h;
	short text_v = center_v;
	
	s = "\p" "Advanced Mac Substitute";
	
	text_h = center_h - StringWidth( s ) / 2u;
	
	MoveTo( text_h, text_v );
	
	DrawString( s );
	
	text_v += 36;
	
	s = "\p" "by Josh Juran";
	
	text_h = center_h - StringWidth( s ) / 2u;
	
	MoveTo( text_h, text_v );
	
	DrawString( s );
	
	text_v += 60;
	
	s = "\p" "https://www.v68k.org/ams/";
	
	text_h = center_h - StringWidth( s ) / 2u;
	
	MoveTo( text_h, text_v );
	
	DrawString( s );
	
	ForeColor( blackColor );
	BackColor( whiteColor );
}

static
WindowRef create_window()
{
	const Rect& bounds = CrsrPin;
	
	WindowRef window;
	
	window = NewWindow( 0, &bounds, "\p", 1, plainDBox, (WindowRef) -1, 0, 0 );
	
	SetPortWindowPort( window );
	
	const Rect& portRect = window->portRect;
	
	RectRgn( window->visRgn, &portRect );
	
	draw_window( window );
	
	ValidRect( &portRect );
	
	return window;
}

int main()
{
	Boolean quitting = false;
	
#if ! TARGET_API_MAC_CARBON
	
	InitGraf( &qd.thePort );
	
	InitFonts();
	InitWindows();
	InitMenus();
	
	InitCursor();
	HideCursor();
	
#endif
	
	WindowRef main_window = create_window();
	
	while ( ! quitting )
	{
		EventRecord event;
		
		if ( WaitNextEvent( everyEvent, &event, 0x7FFFFFFF, NULL ) )
		{
			WindowRef window;
			
			switch ( event.what )
			{
				case mouseDown:
				case keyDown:
					quitting = true;
					break;
				
				case updateEvt:
					window = (WindowRef) event.message;
					
					BeginUpdate( window );
					draw_window( window );
					EndUpdate  ( window );
					break;
				
				default:
					break;
			}
		}
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

/*
	window.cc
	---------
	
	Sound Check for classic Mac OS
	
	Copyright 2025, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	Sound Check tests the Sound Driver API.
	
	This module defines the behavior of the main Sound Check window.
	
*/

#include "window.hh"

// Mac OS
#ifndef __CONTROLDEFINITIONS__
#include <ControlDefinitions.h>
#endif
#ifndef __CONTROLS__
#include <Controls.h>
#endif
#ifndef __NUMBERFORMATTING__
#include <NumberFormatting.h>
#endif
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// mac-qd-utils
#include "mac_qd/main_display_bounds.hh"

// mac-app-utils
#include "mac_app/new_window.hh"

// SoundCheck
#include "UI.hh"


#pragma exceptions off


enum
{
	window_width  = 300,
	window_height = 135,
};

static Rect button_rect =
{
	window_height - 35, window_width / 2 - 35,
	window_height - 15, window_width / 2 + 35,
};

static inline
Rect get_window_bounds()
{
	Rect bounds = mac::qd::main_display_bounds();
	
	bounds.top    += 40;  // approximate menu bar height + title bar height
	bounds.bottom -=  2;  // approximate window frame thickness
	
	short desk_height = bounds.bottom - bounds.top;
	short desk_width  = bounds.right - bounds.left;
	
	short v_margin = (desk_height - window_height) / 2u;
	short h_margin = (desk_width  - window_width ) / 2u;
	
	bounds.top += v_margin;
	bounds.left += h_margin;
	bounds.right = bounds.left + window_width;
	bounds.bottom = bounds.top + window_height;
	
	return bounds;
}

static inline
WindowRef new_window( ConstStr255Param name )
{
	using mac::app::new_window;
	
	Rect bounds = get_window_bounds();
	
	const WindowRef behind = (WindowRef) -1;
	const short     procid = noGrowDocProc;
	
	return new_window( bounds, name, true, procid, behind, true );
}

static inline
void populate_OSErr_string( Byte* s, OSErr err )
{
	Byte* p = s + 1;
	
	*p++ = 'O';
	*p++ = 'S';
	*p++ = 'E';
	*p++ = 'r';
	*p++ = 'r';
	
	NumToString( err, p );
	
	s[ 0 ] = 6 + *p;
	
	*p = ' ';
}

typedef void (*button_action_proc)();

void create_window()
{
	console_window = new_window( "\p" "Sound Check" );
	
	button_action_proc action_proc = &play;
	
	play_stop_button = NewControl( console_window,
	                               &button_rect,
	                               play_button_name,
	                               true,
	                               0,
	                               0,
	                               1,
	                               pushButProc,
	                               (long) action_proc );
	
	InsetRect( &button_rect, -4, -4 );
}

void draw_window( WindowRef window )
{
	enum
	{
		h_margin  = 23,
		text_left = h_margin,
	};
	
	EraseRect( &window->portRect );
	
	DrawControls( window );
	
	PenSize( 3, 3 );
	
	FrameRoundRect( &button_rect, 16, 16 );
	
	PenNormal();
	
	TextFont(  0 );
	TextSize( 12 );
	
	short text_baseline = 36;
	
	MoveTo( text_left, text_baseline );
	
	DrawString( "\p" "This is a basic test of the Sound Driver." );
	
	if ( write_error )
	{
		Str15 write_error_buffer;
		
		populate_OSErr_string( write_error_buffer, write_error );
		
		TextFont( kFontIDGeneva );
		TextSize( 9 );
		
		text_baseline = window_height - 72 + 15;
		
		MoveTo( text_left, text_baseline );
		
		TextFace( bold );
		DrawString( "\p" "Error: " );
		
		ForeColor( redColor );
		TextFace( normal );
		DrawString( write_error_buffer );
		
		ForeColor( blackColor );
	}
	
	Rect dotted_line =
	{
		window_height - 51,                h_margin,
		window_height - 50, window_width - h_margin,
	};
	
	FillRect( &dotted_line, &qd.gray );
}

void content_click( WindowRef window, Point where )
{
	ControlRef control;
	ControlPartCode part;
	
	part = FindControl( where, window, &control );
	
	if ( part != kControlNoPart )
	{
		part = TrackControl( control, where, NULL );
		
		if ( part == kControlButtonPart )
		{
			if ( long ref = GetControlReference( control ) )
			{
				button_action_proc proc = (button_action_proc) ref;
				
				proc();
			}
		}
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

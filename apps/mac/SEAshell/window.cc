/*
	window.cc
	---------
	
	SEA Shell for classic Mac OS
	
	Copyright 2025, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	SEA Shell is the core of a self-extracting archive (SEA).
	
	This module defines the behavior of the main SEA Shell window.
	
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
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// mac-qd-utils
#include "mac_qd/get_portRect.hh"
#include "mac_qd/main_display_bounds.hh"
#include "mac_qd/plot_icon_id.hh"

// mac-app-utils
#include "mac_app/new_window.hh"
#include "mac_app/state.hh"

// Legacynth
#include "UI.hh"


#pragma exceptions off


using mac::qd::get_portRect;


enum
{
	window_width  = 300,
	window_height = 180,
};

static Rect button_rect =
{
	window_height - 35, window_width / 2 - 35,
	window_height - 15, window_width / 2 + 35,
};

static
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

static const Byte* product_name_string = "\p";

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

void quit()
{
	mac::app::quitting = true;
}

void create_window()
{
	if ( Handle h = Get1Resource( 'STR ', 256 ) )
	{
		HLock( h );
		
		product_name_string = (Byte*) *h;
	}
	
	const Byte* window_title = product_name_string;
	
	if ( Handle h = Get1Resource( 'STR ', 257 ) )
	{
		HLock( h );
		
		window_title = (Byte*) *h;
	}
	
	console_window = new_window( window_title );
	
	button_action_proc action_proc = &install;
	
	install_button = NewControl( console_window,
	                             &button_rect,
	                             "\p" "Install" "\xC9",  // ellipsis...
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
		h_margin = 23,
		text_left = h_margin,
		text_line_height = 15,
	};
	
	short text_baseline = 3 + 12 + 9;
	
	const Rect& portRect = get_portRect( window );
	
	EraseRect( &portRect );
	
	DrawControls( window );
	
	PenSize( 3, 3 );
	
	FrameRoundRect( &button_rect, 16, 16 );
	
	PenNormal();
	
	TextSize( 9 );
	
	short centered_left;
	
	const Byte* line_1 = "\p" "This is a self-extracting archive";
	const Byte* line_2 = "\p" "made with SEA Shell, by Josh Juran.";
	
	centered_left = (window_width - StringWidth( line_1 )) / 2;
	
	MoveTo( centered_left, text_baseline );
	
		DrawString( line_1 );
	
	text_baseline += text_line_height;
	
	centered_left = (window_width - StringWidth( line_2 )) / 2;
	
	MoveTo( centered_left, text_baseline );
	
		DrawString( line_2 );
	
	if ( install_error )
	{
		text_baseline = window_height - 72 + text_line_height;
		
		MoveTo( text_left, text_baseline );
		
		Str15 install_error_buffer;
		
		populate_OSErr_string( install_error_buffer, install_error );
		
		TextFace( bold );
		DrawString( "\p" "Installation error: " );
		
		ForeColor( redColor );
		TextFace( normal );
		DrawString( install_error_buffer );
		
		ForeColor( blackColor );
	}
	
	centered_left = (window_width - StringWidth( product_name_string )) / 2;
	
	short icon_delta = 90;
	
	MoveTo( centered_left, window_height - icon_delta + 12 );
	
	DrawString( product_name_string );
	
	Rect icon_rect =
	{
		window_height - icon_delta - 32, window_width / 2 - 16,
		window_height - icon_delta,      window_width / 2 + 16,
	};
	
	mac::qd::plot_icon_id( icon_rect, 256 );
	
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

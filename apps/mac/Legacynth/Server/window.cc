/*
	window.cc
	---------
	
	Legacynth Server for classic Mac OS
	
	Copyright 2024-2025, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	Legacynth implements the Sound Driver API in traditional Mac OS
	environments that don't.
	
	This application is a sound server and API adapter, converting
	I/O requests to the Sound Driver into Sound Manager commands.
	
	This module defines the behavior of the Legacynth Console window.
	
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
#ifndef __SOUND__
#include <Sound.h>
#endif

// mac-sys-utils
#include "mac_sys/get_address_mask.hh"
#include "mac_sys/trap_available.hh"

// mac-qd-utils
#include "mac_qd/get_portRect.hh"
#include "mac_qd/globals/arrow.hh"
#include "mac_qd/globals/thePort_window.hh"
#include "mac_qd/main_display_bounds.hh"

// mac-ui-utils
#include "mac_ui/menus.hh"
#include "mac_ui/windows.hh"

// mac-app-utils
#include "mac_app/state.hh"

// Legacynth
#include "status.hh"
#include "UI.hh"


#pragma exceptions off


#define LOWMEM( addr, type )  (*(type*) (addr))

#define UTableBase  LOWMEM( 0x011C, Handle* )
#define SoundDCE    LOWMEM( 0x027A, void*   )
#define ROM85       LOWMEM( 0x028E, short   )


using mac::qd::get_portRect;


static
Rect get_window_bounds()
{
	enum
	{
		window_width = 300,
		window_height = 150,
	};
	
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
	Rect bounds = get_window_bounds();
	
	const WindowRef behind = (WindowRef) -1;
	const short     procid = noGrowDocProc;
	
	return NewWindow( NULL, &bounds, name, true, procid, behind, true, 0 );
}

static const Byte cant_be_used[] = "\p" "can't be used on this system";
static const Byte not_installed[] = "\p" "not installed";
static const Byte unknown[]       = "\p" "unknown";

static const Byte idle[]       = "\p" "idle";
static const Byte fourtone[]   = "\p" "four-tone";
static const Byte freeform[]   = "\p" "free-form";
static const Byte squarewave[] = "\p" "square-wave";

static Str15 app_version_buffer;
static Str15 ext_version_buffer;
static Str15 snd_manager_buffer;
static Str15 start_error_buffer;

static const Byte* dev_control_string;
static const Byte* synthesizer_string;
static const Byte* ext_version_string;
static const Byte* snd_manager_string;

static Style dev_control_style;
static Style synthesizer_style;
static Style ext_version_style;
static Style snd_manager_style;

static UInt32 synthesizer_color;
static UInt32 ext_version_color;

static Byte start_stop_hilite;

static
bool has_SoundManager()
{
	enum
	{
		_SoundDispatch = 0xA800,
	};
	
	if ( ! TARGET_CPU_68K )
	{
		return true;
	}
	
	return ROM85 > 0  &&  mac::sys::trap_available( _SoundDispatch );
}

static
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

static
void populate_version_string( Byte* s, short vers )
{
	int major = vers >> 8;
	int minor = vers >> 4 & 0xF;
	int point = vers      & 0xF;
	
	Byte* p = s + 1;
	
	if ( major > 9 )
	{
		*p++ = '0' + major / 10;
		
		major %= 10;
	}
	
	*p++ = '0' + major;
	*p++ = '.';
	
	if ( minor > 9 )
	{
		*p++ = '0' + minor / 10;
		
		minor %= 10;
	}
	
	*p++ = '0' + minor;
	
	if ( point )
	{
		*p++ = '.';
		
		if ( point > 9 )
		{
			*p++ = '0' + point / 10;
			
			point %= 10;
		}
		
		*p++ = '0' + point;
	}
	
	s[ 0 ] = p - (s + 1);
}

typedef void (*button_action_proc)();

static
void quit()
{
	mac::app::quitting = true;
}

void create_window( OSErr err )
{
	console_window = new_window( "\p" "Legacynth Console" );
	
	Rect button_rect = { 115, 120, 135, 180 };
	
	const Byte* title = err < 0 ? "\p" "Quit"
	                            : start_button_name;
	
	button_action_proc action_proc = err < 0 ? &quit : &startup;
	
	start_stop_button = NewControl( console_window,
	                                &button_rect,
	                                title,
	                                true,
	                                0,
	                                0,
	                                1,
	                                pushButProc,
	                                (long) action_proc );
	
	if ( Handle h = Get1Resource( 'vers', 1 ) )
	{
		populate_version_string( app_version_buffer, *(short*) *h );
		
		ReleaseResource( h );
	}
	
	ext_version_color = blackColor;
	
	if ( extension_software_version )
	{
		short vers = *(short*) &extension_software_version;
		
		populate_version_string( ext_version_buffer, vers );
		
		ext_version_string = ext_version_buffer;
	}
	else if ( err == 0 )
	{
		ext_version_string = unknown;
		ext_version_style  = italic;
	}
	else if ( err == dupFNErr )
	{
		ext_version_string = cant_be_used;
		ext_version_style  = italic;
		ext_version_color  = redColor;
	}
	else if ( err == unitEmptyErr  ||  err == noHardwareErr )
	{
		ext_version_string = not_installed;
		ext_version_style  = italic;
	}
	else
	{
		populate_OSErr_string( ext_version_buffer, err );
		
		ext_version_string = ext_version_buffer;
	}
	
	if ( has_SoundManager() )
	{
		NumVersion version = SndSoundManagerVersion();
		
		short vers = *(short*) &version;
		
		populate_version_string( snd_manager_buffer, vers );
		
		snd_manager_string = snd_manager_buffer;
	}
	else
	{
		snd_manager_string = not_installed;
		snd_manager_style  = italic;
	}
}

static inline
bool same_address( const void* a, const void* b )
{
	long mask = mac::sys::get_address_mask();
	
	return (((long) a ^ (long) b) & mask) == 0;
}

static
void check_device_controller()
{
	synthesizer_string = "\p";
	synthesizer_style  = normal;
	synthesizer_color  = blackColor;
	
	start_stop_hilite = 0;
	
	if ( running )
	{
		dev_control_string = "\p" "Legacynth";
		
		if ( ! sound_active )
		{
			synthesizer_string = idle;
		}
		else if ( sound_mode == 1 )
		{
			synthesizer_string = fourtone;
		}
		else if ( sound_mode == 0 )
		{
			synthesizer_string = freeform;
		}
		else if ( sound_mode == -1 )
		{
			synthesizer_string = squarewave;
		}
		else
		{
			synthesizer_string = "\p" "???";
		}
	}
	else if ( (long) SoundDCE == -1 )
	{
		dev_control_style = normal;
		
		dev_control_string = "\p" "none";
	}
	else if ( same_address( SoundDCE, *UTableBase[ 3 ] ) )
	{
		dev_control_string = "\p" "Sound Driver";
	}
	else
	{
		dev_control_style = italic;
		
		dev_control_string = "\p" "occupied";
		
		start_stop_hilite = kControlInactivePart;
	}
	
	if ( startup_error )
	{
		populate_OSErr_string( start_error_buffer, startup_error );
		
		synthesizer_string = start_error_buffer;
		
		synthesizer_style = italic;
		synthesizer_color = redColor;
	}
}

void draw_window( WindowRef window )
{
	enum
	{
		text_left = 26,
		text_line_height = 15,
	};
	
	check_device_controller();
	
	short text_baseline = 3 + 12 + 9;
	
	const Rect& portRect = get_portRect( window );
	
	EraseRect( &portRect );
	
	HiliteControl( start_stop_button, start_stop_hilite );
	
	DrawControls( window );
	
	SetPortWindowPort( window );
	
	TextSize( 9 );
	
	MoveTo( text_left, text_baseline );
	
		TextFace( bold );
		DrawString( "\p" "Driver software: " );
		
		TextFace( dev_control_style );
		DrawString( dev_control_string );
	
	text_baseline += text_line_height;
	
	MoveTo( text_left, text_baseline );
	
		TextFace( bold );
		DrawString( "\p" "Current synth: " );
		
		TextFace( synthesizer_style );
		ForeColor( synthesizer_color );
		DrawString( synthesizer_string );
		
		ForeColor( blackColor );
	
	text_baseline += text_line_height;
	
	MoveTo( text_left, text_baseline );
	
		TextFace( bold );
		DrawString( "\p" "Server version: " );
		
		TextFace( normal );
		DrawString( app_version_buffer );
	
	text_baseline += text_line_height;
	
	MoveTo( text_left, text_baseline );
	
		TextFace( bold );
		DrawString( "\p" "Extension version: " );
		
		TextFace( ext_version_style );
		ForeColor( ext_version_color );
		DrawString( ext_version_string );
		
		ForeColor( blackColor );
	
	text_baseline += text_line_height;
	
	MoveTo( text_left, text_baseline );
	
		TextFace( bold );
		DrawString( "\p" "Sound Manager version: " );
		
		TextFace( snd_manager_style );
		DrawString( snd_manager_string );
	
	Rect dotted_line = { 99, 26, 100, 274 };
	
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

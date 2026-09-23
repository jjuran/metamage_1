/*
	BitsAndTextFadeInDemo.cc
	------------------------
	
	Bits and text fade-in demo for Advanced Mac Substitute
	
	Copyright 2026, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
*/

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// production-logo-app-utils
#include "init.hh"
#include "terminate.hh"
#include "wait.hh"
#include "window.hh"

// BitsAndTextFadeInDemo
#include "bits.hh"
#include "text.hh"


int main()
{
	init();
	
	WindowRef window = create_window();
	
	const Rect& portRect = window->portRect;
	
	const short window_width  = portRect.right - portRect.left;
	const short window_height = portRect.bottom - portRect.top;
	
	TextFont(  3 );  // Geneva
	TextSize( 18 );  // 9pt @2x
	
	/*
		Animation begins here.
	*/
	
	wait_or_exit( 30 );
	
	const short center_h = window_width  / 2u;
	const short center_v = window_height / 2u;
	
	fade_in_bits( center_h, center_v + 14 );
	
	const Byte* text = "\p" "All your bits are belong to us";
	const Byte* tail = "\p" ".";
	
	short text_width = StringWidth( text );
	
	short text_h = center_h - text_width / 2u;
	short text_v = center_v + 36 + 1;
	
	MoveTo( text_h, text_v );
	
	inscribe( text );
	inscribe( tail );
	
	wait_or_exit( 60 * 3 );
	
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

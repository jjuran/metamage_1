/*
	SystemsTwilightTuneup.cc
	------------------------
	
	System's Twilight Tune-up INIT for Advanced Mac Substitute
	
	Copyright 2022, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	System's Twilight has various screens with animations that run in the
	background.  Some of them (e.g. open gates) draw concentric circles
	without any buffering -- the circular space is cleared, and then up
	to six circles are drawn via FrameOval().  This results in visible
	flickering, which is not just unsightly, but may pose a usability or
	even a safety issue for users with certain neurological conditions.
	
	This extension patches the jump table in order to wrap calls to the
	animation function.  By bracketing the animation calls with callouts
	that defer screen updates, we prevent the flickering entirely.
	
	DISCLAIMER:  No medical professionals were consulted regarding the
	development of this program.  None of the information contained herein
	constitutes medical advice.  I make no guarantee that this program
	will work as described or make System's Twilight safe for you to use.
	Use at your own risk.
	
*/

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif
#ifndef __TRAPS__
#include <Traps.h>
#endif

// mac-sys-utils
#include "mac_sys/load_segment.hh"
#include "mac_sys/trap_address.hh"

// ams-common
#include "callouts.hh"


#pragma exceptions off


Ptr CurrentA5 : 0x0904;

typedef void (*generic_code)();

static generic_code      animator;
static UniversalProcPtr  old_TEInit;

static
void animate_buffered()
{
	lock_screen();
	
	animator();
	
	unlock_screen();
}

static inline
void install_patch()
{
	mac::sys::load_segment( 5 );
	
	const int entry_offset = 1146;
	const Ptr function_loc = CurrentA5 + entry_offset + 2;
	
	generic_code& jump_target = *(generic_code*) function_loc;
	
	animator = jump_target;
	
	jump_target = &animate_buffered;
}

static
pascal
void TEInit_patch()
{
	if ( Handle h = Get1Resource( 'STwZ', 0 ) )
	{
		ReleaseResource( h );
		
		install_patch();
	}
	
	old_TEInit();
}

int main()
{
	old_TEInit = mac::sys::get_trap_address( _TEInit );
	
	mac::sys::set_trap_address( (ProcPtr) TEInit_patch, _TEInit );
	
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

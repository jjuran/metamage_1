/*
	UI.cc
	-----
	
	Sound Check for classic Mac OS
	
	Copyright 2025, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	Sound Check tests the Sound Driver API.
	
	This module defines the UI-observable details of the two
	state transitions: starting playback and stopping it.
	
*/

#include "UI.hh"

// Mac OS
#ifndef __CONTROLS__
#include <Controls.h>
#endif
#ifndef __CONTROLDEFINITIONS__
#include <ControlDefinitions.h>
#endif

// mac-glue-utils
#include "mac_glue/OSUtils.hh"

// mac-ui-utils
#include "mac_ui/windows.hh"

// SoundCheck
#include "playback.hh"


#pragma exceptions off


const Byte play_button_name[] = "\p" "Test";
const Byte stop_button_name[] = "\p" "Stop";

WindowRef  console_window;
ControlRef play_stop_button;

OSErr write_error;

void play()
{
	write_error = start_playback();
	
	if ( write_error == noErr  &&  playing )
	{
		SetControlTitle    ( play_stop_button, stop_button_name );
		SetControlReference( play_stop_button, (long) &stop );
	}
	
	mac::ui::invalidate_window( console_window );
}

void stop()
{
	stop_playback();
	
	SetControlTitle    ( play_stop_button, play_button_name );
	SetControlReference( play_stop_button, (long) &play );
	
	mac::ui::invalidate_window( console_window );
}

void default_button_click()
{
	HiliteControl( play_stop_button, kControlButtonPart );
	
	mac::glue::delay( 8 );
	
	HiliteControl( play_stop_button, kControlNoPart );
	
	if ( playing )
	{
		stop();
	}
	else
	{
		play();
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

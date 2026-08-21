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

// mac-ui-utils
#include "mac_ui/invoke_button.hh"

// mac-app-utils
#include "mac_app/invalidate_window.hh"

// SoundCheck
#include "playback.hh"


#pragma exceptions off


using mac::app::invalidate_window;


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
	
	invalidate_window( console_window );
}

void stop()
{
	stop_playback();
	
	SetControlTitle    ( play_stop_button, play_button_name );
	SetControlReference( play_stop_button, (long) &play );
	
	invalidate_window( console_window );
}

void default_button_click()
{
	mac::ui::invoke_button_inline( play_stop_button );
	
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

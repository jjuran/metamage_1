/*
	UI.cc
	-----
	
	Legacynth Server for classic Mac OS
	
	Copyright 2024, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	Legacynth implements the Sound Driver API in traditional Mac OS
	environments that don't.
	
	This application is a sound server and API adapter, converting
	I/O requests to the Sound Driver into Sound Manager commands.
	
	This module defines the UI-observable details of the two
	state transitions: server startup and shutdown.
	
*/

#include "UI.hh"

// Mac OS
#ifndef __CONTROLS__
#include <Controls.h>
#endif
#ifndef __MENUS__
#include <Menus.h>
#endif

// mac-ui-utils
#include "mac_ui/windows.hh"

// Legacynth
#include "Sound.hh"
#include "status.hh"


#pragma exceptions off


const Byte start_button_name[] = "\p" "Start";
const Byte stop_button_name [] = "\p" "Stop";

WindowRef  console_window;
ControlRef start_stop_button;

void startup()
{
	startup_error = Sound_startup();
	
	if ( startup_error == noErr )
	{
		running = true;
		
		SetControlTitle    ( start_stop_button, stop_button_name );
		SetControlReference( start_stop_button, (long) &cleanup );
		
		MenuRef fileMenu = GetMenuHandle( File );
		
		DisableItem( fileMenu, Start );
		EnableItem ( fileMenu, Stop  );
	}
	
	mac::ui::invalidate_window( console_window );
}

void cleanup()
{
	if ( Sound_cleanup() == noErr )
	{
		running = false;
		
		SetControlTitle    ( start_stop_button, start_button_name );
		SetControlReference( start_stop_button, (long) &startup );
		
		MenuRef fileMenu = GetMenuHandle( File );
		
		DisableItem( fileMenu, Stop  );
		EnableItem ( fileMenu, Start );
		
		mac::ui::invalidate_window( console_window );
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

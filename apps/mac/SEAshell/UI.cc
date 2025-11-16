/*
	UI.cc
	-----
	
	SEA Shell for classic Mac OS
	
	Copyright 2025, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	SEA Shell is the core of a self-extracting archive (SEA).
	
	This module defines the UI of the Install button.
	
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

// mac-sys-utils
#include "mac_sys/beep.hh"

// mac-ui-utils
#include "mac_ui/windows.hh"

// SEAshell
#include "install.hh"
#include "location.hh"
#include "payload.hh"


#pragma exceptions off


WindowRef  console_window;
ControlRef install_button;

OSErr install_error;

bool installed;

void install()
{
	OSErr err;
	
	err = ready_payload();
	
	if ( err == noErr )
	{
		err = choose_install_location();
		
		if ( err == userCanceledErr )
		{
			return;
		}
		
		if ( err == noErr )
		{
			err = install_to( install_location );
			
			if ( err == noErr )
			{
				SetControlTitle    ( install_button, "\p" "Quit"  );
				SetControlReference( install_button, (long) &quit );
				
				installed = true;
			}
		}
	}
	
	if ( err != install_error )
	{
		mac::ui::invalidate_window( console_window );
	}
	
	if ( (install_error = err) )
	{
		mac::sys::beep();
	}
}

void default_button_click()
{
	HiliteControl( install_button, kControlButtonPart );
	
	mac::glue::delay( 8 );
	
	HiliteControl( install_button, kControlNoPart );
	
	if ( installed )
	{
		quit();
	}
	else
	{
		install();
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

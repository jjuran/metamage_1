/*
	payload.cc
	----------
	
	SEA Shell for classic Mac OS
	
	Copyright 2025, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	SEA Shell is the core of a self-extracting archive (SEA).
	
	This module prepares the stowed archive for installation.
	
*/

#include "payload.hh"

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// MacBinaryLib
#include "MacBinary.hh"


unsigned long payload_size;

const Byte* payload_data;
const Byte* payload_name;

OSErr ready_payload()
{
	if ( Handle h = Get1Resource( 'BIN+', 256 ) )
	{
		HLock( h );
		
		payload_size = GetHandleSize( h );
		payload_data = (Byte*) *h;
		payload_name = payload_data + 1;
		
		return noErr;
	}
	
	OSErr err = ResError();
	
	return err ? err : resNotFound;
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

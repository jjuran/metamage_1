/*
	location.cc
	-----------
	
	SEA Shell for classic Mac OS
	
	Copyright 2025, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	SEA Shell is the core of a self-extracting archive (SEA).
	
	This module implements selection of the install location.
	
*/

#include "location.hh"

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// mac-sys-utils
#include "mac_sys/gestalt.hh"

// mac-app-utils
#include "mac_app/file_save_dialog.hh"

// SEAshell
#include "payload.hh"


#pragma exceptions off


#define PROMPT "\p" "Choose an install location:"


FSSpec install_location;

static inline
void my_memcpy( void* dst, const void* src, unsigned n )
{
	BlockMoveData( src, dst, n );
}

static
long FSSpec_saver( const FSSpec& file )
{
	my_memcpy( &install_location, &file, sizeof file );
	
	return noErr;
}

static
long HFS_file_saver( short vRefNum, long dirID, const Byte* name )
{
	install_location.vRefNum = vRefNum;
	install_location.parID   = dirID;
	
	my_memcpy( install_location.name, name, 1 + name[ 0 ] );
	
	return noErr;
}

OSErr choose_install_location()
{
	using mac::app::file_save_dialog;
	
	enum
	{
		gestaltStandardFileAttr = 'stdf',
		
		gestaltStandardFile58   = 0,
	};
	
	const bool has_StandardFile_5_thru_8 =
		mac::sys::gestalt_bit_set( gestaltStandardFileAttr,
		                           gestaltStandardFile58 );
	
	OSErr err;
	
	if ( has_StandardFile_5_thru_8 )
	{
		err = file_save_dialog( PROMPT, payload_name, &FSSpec_saver );
	}
	else
	{
		err = file_save_dialog( PROMPT, payload_name, &HFS_file_saver );
	}
	
	if ( err == noErr )
	{
		payload_name = install_location.name;
	}
	
	return err;
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

/*
	common.hh
	---------
	
	Code shared by Legacynth's INIT and server application.
	
	Copyright 2022, 2024-2025, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	Legacynth implements the Sound Driver API in traditional Mac OS
	environments that don't.
	
*/

#ifndef LEGACYNTH_COMMON_HH
#define LEGACYNTH_COMMON_HH

// Mac OS
#ifndef __DEVICES__
#include <Devices.h>
#endif
#ifndef __GESTALT__
#include <Gestalt.h>
#endif


UInt8 CPUFlag    : 0x012F;
short SysVersion : 0x015A;


enum
{
	kSoundDriverRefNum = -4,
	
	kLegacynthQueryInstalled = (short) 0x1e9a,
	kLegacynthSetBaseAddress = (short) 0xBA5E,
};

struct exported_state
{
	void* a5;
	
	void* write;
	void* killIO;
};

inline
long gestalt( unsigned long selector, long default_value = 0 )
{
	SInt32 result;
	
	const OSErr err = ::Gestalt( selector, &result );
	
	return err == noErr ? result : default_value;
}

inline
bool is_68K()
{
	return gestalt( 'sysa' ) < gestaltPowerPC;
}

inline
bool is_AV_Mac()
{
	long mach = gestalt( 'mach' );
	
	return mach == gestaltMacQuadra660AV  ||  mach == gestaltMacQuadra840AV;
}

inline
bool has_dot_Sound()
{
	DCtlHandle dceHandle = GetDCtlEntry( kSoundDriverRefNum );
	
	return dceHandle != NULL;
}

inline
bool has_Sound_Driver()
{
	/*
		Anything less than a 68020 or running System 6 (or earlier)
		already has a working and usable Sound Driver.
	*/
	
	if ( CPUFlag < 2  ||  SysVersion < 0x700 )
	{
		return true;
	}
	
	/*
		Since we have at least System 7, we can call Gestalt().
		
		We can't just check for the Sound Driver in the unit table,
		because Mac OS 9 continues to provide a dummy .Sound driver
		that doesn't actually implement the Sound Driver API.
		
		PPC and AV Macs are known to lack support.  Basilisk II
		somehow doesn't have the .Sound driver installed, so in
		that case it's useful to check for it.
		
		If this is a non-AV 68K Mac with the .Sound driver
		installed, we'll assume that it works.
	*/
	
	if ( is_68K()  &&  ! is_AV_Mac()  &&  has_dot_Sound() )
	{
		return true;
	}
	
	/*
		The vendor left out Sound Driver support in this system.
		(Third-party opportunity!)
	*/
	
	return false;
}

#endif

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

/*
	LRMusicFix.cc
	-------------
	
	Lode Runner Music Fix INIT for Advanced Mac Substitute
	
	Copyright 2020, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	This is a quick and dirty system patch to fix four-tone playback in
	Lode Runner when run in the Advanced Mac Substitute environment.
	
	The problem is that Lode Runner populates the four-tone sound record
	with triangle waves running from 00 to FE and back to 00, but with a
	phase of zero, which corresponds to physical level 00 but logical level
	-128, as opposed to a quarter rotation later, at physical level 80 and
	logical level 0.  The jump from silence at level 0 to the wave start at
	level -128 is audible as a popping noise.
	
	This program overwrites the four phase fields in Lode Runner's global
	variables, setting each voice's phase to 64, avoiding the level jump.
	
	This code is designed to run in Advanced Mac Substitute.  If Lode Runner
	is found to exhibit the same audio defect on real hardware, or in another
	emulator with otherwise usable sound output, then this program can be
	extended to support Mac OS.
	
*/

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif
#ifndef __TRAPS__
#include <Traps.h>
#endif


#pragma exceptions off


static UniversalProcPtr old_VInstall;
static UniversalProcPtr old_TEInit;


static
asm void VInstall_patch()
{
	MOVEQ.L  #64,D0
	MOVE.L   D0,-218(A5)  // soundRec.sound1Phase = 64
	MOVE.L   D0,-210(A5)  // soundRec.sound2Phase = 64
	MOVE.L   D0,-202(A5)  // soundRec.sound3Phase = 64
	MOVE.L   D0,-194(A5)  // soundRec.sound4Phase = 64
	
	MOVE.L   old_VInstall,-(SP)
	RTS
}

static
void TEInit_handler()
{
	if ( Handle h = Get1Resource( 'GLEN', 0 ) )
	{
		ReleaseResource( h );
		
		old_VInstall = NGetTrapAddress( _VInstall, OSTrap );
		
		NSetTrapAddress( (UniversalProcPtr) VInstall_patch, _VInstall, OSTrap );
	}
}

static
pascal asm void TEInit_patch()
{
	LINK     A6,#0
	JSR      TEInit_handler
	UNLK     A6
	
	MOVEA.L  old_TEInit,A0
	JMP      (A0)
}

int main()
{
	old_TEInit = NGetTrapAddress( _TEInit, ToolTrap );
	
	NSetTrapAddress( (UniversalProcPtr) TEInit_patch, _TEInit, ToolTrap );
	
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

/*
	LegacynthINIT.cc
	----------------
	
	Legacynth INIT for classic Mac OS
	
	Copyright 2022, 2024-2025, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	Legacynth implements the Sound Driver API in traditional Mac OS
	environments that don't.
	
	This extension patches the Write, Control, and KillIO traps
	in order to service I/O requests to the Sound Driver.
	
	While installing a driver into the unit table might seem like a
	more reasonable option (and wouldn't require any trap patches),
	in some later versions of Mac OS it runs afoul of the serial port
	arbitrator, which prevents more than one application at a time
	from opening the driver.  A potential future improvement would
	be to check for the serial port arbitrator at startup and only
	patch traps if it's present, and install a driver otherwise.
	
	This extension doesn't have any logic at all related to sound APIs
	or audio generation -- it just patches the I/O traps at startup,
	allowing the Legacynth server application to hook in later.
	
*/

// Mac OS
#ifndef __DEVICES__
#include <Devices.h>
#endif
#ifndef __RESOURCES__
#include <Resources.h>
#endif
#ifndef __TRAPS__
#include <Traps.h>
#endif

// mac-sys-utils
#include "mac_sys/trap_address.hh"

// legacynth-common
#include "legacynth/common.hh"


#pragma exceptions off


enum
{
	kLegacynthINITProtocolVersion = 0 << 16 | 1,
};

static UInt32 version_number;

static exported_state* server_state;

static UniversalProcPtr old_Write;
static UniversalProcPtr old_Control;
static UniversalProcPtr old_KillIO;

static
asm
short Write_patch( IOParam* pb : __A0 )
{
	CMPI.W   #kSoundDriverRefNum,24(A0)
	BEQ.S    Sound
	
pass:
	// call previous Write handler
	MOVE.L   old_Write,-(SP)
	RTS
	
Sound:
	
	MOVE.L   server_state,D2
	BEQ.S    pass
	
	MOVE.W   D1,6(A0)    // set ioTrap
	
	BTST     #10,D1      // async bit
	BNE.S    async
	
	CLR.L    12(A0)      // Clear ioCompletion unless async
	
async:
	
	LINK     A6,#0
	MOVEM.L  A2/A5,-(SP)
	
	MOVEA.L  D2,A2
	MOVEA.L  (A2)+,A5
	MOVEA.L  (A2),A2
	JSR      (A2)
	
	MOVEM.L  (SP)+,A2/A5
	UNLK     A6
	
	RTS
}

static
asm
short Control_patch( CntrlParam* pb : __A0 )
{
	CMPI.W   #kSoundDriverRefNum,24(A0)
	BEQ.S    Sound
	
	// call previous Control handler
	MOVE.L   old_Control,-(SP)
	RTS
	
Sound:
	
	CMPI.W   #kLegacynthQueryInstalled,26(A0)  // csCode
	BEQ.S    query
	
	CMPI.W   #kLegacynthSetBaseAddress,26(A0)  // csCode
	BEQ.S    set_base
	
	MOVEQ.L  #controlErr,D0
	RTS
	
query:
	LEA      28(A0),A1
	MOVE.L   #kLegacynthINITProtocolVersion,(A1)+
	MOVE.L   version_number,(A1)+
	BRA.S    success
	
set_base:
	// Set server state base address
	MOVE.L   28(A0),server_state
	
success:
	MOVEQ.L  #0,D0
	
	RTS
}

static
asm
short KillIO_patch( CntrlParam* pb : __A0 )
{
	CMPI.W   #kSoundDriverRefNum,24(A0)
	BEQ.S    Sound
	
pass:
	// call previous KillIO handler
	MOVE.L   old_KillIO,-(SP)
	RTS
	
Sound:
	
	MOVE.L   server_state,D2
	BEQ.S    pass
	
	LINK     A6,#0
	MOVEM.L  A2/A5,-(SP)
	
	MOVEA.L  D2,A2
	MOVEA.L  (A2),A5
	MOVEA.L  8(A2),A2
	JSR      (A2)
	
	MOVEM.L  (SP)+,A2/A5
	UNLK     A6
	
	RTS
}

static
int install_patches()
{
	if ( has_Sound_Driver() )
	{
		return -1;
	}
	
	/*
		If we got this far, then we have at least System 7,
		so we can call Get1Resource().
	*/
	
	if ( Handle vers = Get1Resource( 'vers', 1 ) )
	{
		version_number = *(UInt32*) *vers;
	}
	
	Handle self = Get1Resource( 'INIT', 0 );
	
	DetachResource( self );
	
	old_Write   = mac::sys::get_trap_address( _Write   );
	old_Control = mac::sys::get_trap_address( _Control );
	old_KillIO  = mac::sys::get_trap_address( _KillIO  );
	
	mac::sys::set_trap_address( (ProcPtr) Write_patch,   _Write   );
	mac::sys::set_trap_address( (ProcPtr) Control_patch, _Control );
	mac::sys::set_trap_address( (ProcPtr) KillIO_patch,  _KillIO  );
	
	return 0;
}

int main()
{
	int err = install_patches();
	
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

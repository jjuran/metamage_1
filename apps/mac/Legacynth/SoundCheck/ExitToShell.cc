/*
	ExitToShell.cc
	--------------
	
	Sound Check for classic Mac OS
	
	Copyright 2025, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	Sound Check tests the Sound Driver API.
	
	This module patches ExitToShell to ensure that we're
	no longer playing sound asynchronously when we exit.
	
*/

#include "ExitToShell.hh"

// Mac OS
#ifndef __MIXEDMODE__
#include <MixedMode.h>
#endif
#ifndef __PROCESSES__
#include <Processes.h>
#endif

// mac-config
#include "mac_config/upp-macros.hh"

// mac-upp-utils
#include "mac_upp/type-macros.h"

// mac-sys-utils
#include "mac_sys/trap_address.hh"

// SoundCheck
#include "playback.hh"


using mac::sys::get_trap_address;
using mac::sys::set_trap_address;

typedef UniversalProcPtr UPP;

enum
{
	_ExitToShell = 0xA9F4,
};


enum
{
	uppExitToShellPatchProcInfo = kPascalStackBased
	                            | RESULT_SIZE( SIZE_CODE( 0 ) ),
};

typedef pascal void (*ExitToShellPatchProcPtr)();

typedef STACK_UPP_TYPE( ExitToShellPatchProcPtr )  ExitToShellPatchUPP;

#if CALL_NOT_IN_CARBON

DEFINE_UPP_CONSTRUCTOR( ExitToShellPatch )
DEFINE_UPP_DESTRUCTOR( ExitToShellPatch )
DEFINE_UPP_VOID_INVOKER_0( ExitToShellPatch )

#endif  // #if CALL_NOT_IN_CARBON


static UniversalProcPtr old_ExitToShell;

static
pascal void ExitToShell_patch()
{
	set_trap_address( old_ExitToShell, _ExitToShell );
	
	stop_playback();
	
	ExitToShell();
}

DEFINE_UPP( ExitToShellPatch, ExitToShell_patch )


void patch_ExitToShell()
{
	old_ExitToShell = get_trap_address( _ExitToShell );
	
	set_trap_address( (UPP) UPP_ARG( ExitToShell_patch ), _ExitToShell );
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

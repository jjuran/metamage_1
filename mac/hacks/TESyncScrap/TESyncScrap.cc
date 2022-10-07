/*
	TESyncScrap.cc
	--------------
	
	TESyncScrap INIT for Mac OS
	
	Copyright 2009, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	This system extension patches TextEdit's scrap routines in order
	to keep the TE scrap and the desk scrap in sync at all times.
	
	Prior to Carbon, Mac developers were advised to synchronize the
	TE scrap with the desk scrap during a layer switch -- that is, when
	an application moved into or out of the foreground.  This will work,
	provided that the foreground application is the only party interested
	in the clipboard contents (since it alone knows whether the current
	clipboard contents are stored in the TE scrap or the desk scrap).
	
	This approach prevents background applications and extension code from
	interacting with the clipboard, but now there's an additional factor
	to consider: paravirtualized clipboard integration when Mac OS is
	running in an emulator.  Host layer switches (between the emulator and
	other host applications that use the clipboard) are imperceptible to
	the guest OS, so the foreground guest application won't synchronize the
	scraps when switching in the host, as it would in the guest.
	
	The problem can be demonstrated in both directions with the Key Caps
	desk accessory.
	
	The solution is to tail-patch TECut and TECopy to call TEToScrap()
	afterward, and head-patch[1] TEPaste to call TEFromScrap() in advance.
	
	This ensures that the desk scrap always contains the current clipboard
	contents, so the host's clipboard integration can copy from and update
	it at will.
	
	[1] Technically, our TEPaste patch is not a proper head-only patch --
	though it could be made into one if rewritten in assembly language.
	
*/

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif
#ifndef __SCRAP__
#include <Scrap.h>
#endif
#ifndef __TEXTEDIT__
#include <TextEdit.h>
#endif
#ifndef __TRAPS__
#include <Traps.h>
#endif

// mac-sys-utils
#include "mac_sys/trap_address.hh"


#pragma exceptions off


typedef pascal void (*TEScrap_ProcPtr)( TEHandle hTE );

static TEScrap_ProcPtr old_TECopy;
static TEScrap_ProcPtr old_TECut;
static TEScrap_ProcPtr old_TEPaste;

static OSErr FlushScrap()
{
	OSErr err = ZeroScrap();
	
	if ( err == noErr )
	{
		err = TEToScrap();
	}
	
	return err;
}

static
pascal
void
TECopy_patch( TEHandle hTE )
{
	old_TECopy( hTE );
	
	FlushScrap();
}

static
pascal
void
TECut_patch( TEHandle hTE )
{
	old_TECut( hTE );
	
	FlushScrap();
}

static
pascal
void
TEPaste_patch( TEHandle hTE )
{
	TEFromScrap();
	
	old_TEPaste( hTE );
}

int main()
{
	Handle self = Get1Resource( 'INIT', 0 );
	
	DetachResource( self );
	
	old_TECopy  = (TEScrap_ProcPtr) mac::sys::get_trap_address( _TECopy  );
	old_TECut   = (TEScrap_ProcPtr) mac::sys::get_trap_address( _TECut   );
	old_TEPaste = (TEScrap_ProcPtr) mac::sys::get_trap_address( _TEPaste );
	
	mac::sys::set_trap_address( (ProcPtr) TECopy_patch,  _TECopy  );
	mac::sys::set_trap_address( (ProcPtr) TECut_patch,   _TECut   );
	mac::sys::set_trap_address( (ProcPtr) TEPaste_patch, _TEPaste );
	
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

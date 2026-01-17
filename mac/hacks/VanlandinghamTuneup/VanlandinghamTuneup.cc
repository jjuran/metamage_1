/*
	VanlandinghamTuneup.cc
	----------------------
	
	Vanlandingham Tune-up INIT for Mac OS
	
	Copyright 2024, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	This is a pair of application hot patches for Vanlandingham.
	One replaces a 20-word busy loop with a 4-word Delay call.
	Another rewrites a dissolve routine to use only non-volatile
	registers across a Toolbox trap call to HideCursor().
	
	Although Vanlandingham's "sleep" function is only called
	once at startup (for 10 ticks) and once on Quit (for 12 ticks),
	the patch does measurably reduce CPU time by those amounts
	when run in Advanced Mac Substitute.
	
	HideCursor() is a Toolbox trap call, which is allowed to clobber
	the usual volatile registers: D0-D2 and A0-A1.  In practice,
	the implementation in Advanced Mac Substitute would clobber A0
	had it not been written in assembly language to avoid this very
	outcome.  This patch allows the asm rewrite to be reverted.
	
	(Earlier in the development of Advanced Mac Substitute, the
	HideCursor() implementation did clobber other registers, which
	caused Vanlandingham to crash, necessitating the asm rewrite.
	Now, the only symptom would be drawing garbage to the screen
	on account of the clobbered source address.  Presumably, Mac OS'
	HideCursor() implementation in early ROMs doesn't clobber any of
	the volatile registers (except possibly D0, which Vanlandingham
	doesn't use here), but nothing prevents an INIT from patching it
	with its own code that does.
	
*/

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// mac-glue-utils
#include "mac_glue/Memory.hh"

// mac-sys-utils
#include "mac_sys/has/virtualization.hh"
#include "mac_sys/trap_address.hh"

// VanlandinghamTuneup
#include "dissolve_bytes.hh"
#include "sleep.hh"


#pragma exceptions off


#define LENGTH(array)  (sizeof (array) / sizeof (array)[0])

#define VEC_LEN(array)   array, LENGTH (array)
#define VEC_SIZE(array)  array, sizeof (array)


static UniversalProcPtr old_InitFonts;

static inline
void my_memcpy( void* dst, const void* src, long n )
{
	BlockMoveData( src, dst, n );
}

static inline
bool equal_words( const UInt16* a, const UInt16* b, short n )
{
	do
	{
		if ( *a++ != *b++ )
		{
			return false;
		}
	}
	while ( --n > 0 );
	
	return true;
}

static inline
void install_sleep_patch( Handle h, Size handle_size )
{
	if ( handle_size >= 0x000104 )
	{
		UInt16* p = (UInt16*) (*h + 0x0000dc);
		
		if ( equal_words( p, VEC_LEN( shipped_sleep_loop ) ) )
		{
			my_memcpy( p, VEC_SIZE( patched_sleep_trap ) );
		}
	}
}

static inline
void install_dissolve_bytes_patch( Handle h, Size handle_size )
{
	if ( handle_size >= 0x0025f2 )
	{
		UInt16* pA = (UInt16*) (*h + 0x002566);
		UInt16* pB = (UInt16*) (*h + 0x0025c4);
		
		bool patch_dissolve_bytes =
			equal_words( pA, VEC_LEN( shipped_dissolve_bytes_A ) )  &&
			equal_words( pB, VEC_LEN( shipped_dissolve_bytes_B ) );
		
		if ( patch_dissolve_bytes )
		{
			my_memcpy( pA, VEC_SIZE( patched_dissolve_bytes_A ) );
			my_memcpy( pB, VEC_SIZE( patched_dissolve_bytes_B ) );
		}
	}
}

static
void InitFonts_handler()
{
	if ( Handle h = Get1Resource( 'VANL', 0 ) )
	{
		ReleaseResource( h );
		
		if ( (h = Get1Resource( 'CODE', 1 )) )
		{
			Size size = mac::glue::GetHandleSize_raw( h );
			
			install_sleep_patch( h, size );
			
			install_dissolve_bytes_patch( h, size );
		}
	}
}

static
pascal asm void InitFonts_patch()
{
	LINK     A6,#0
	JSR      InitFonts_handler
	UNLK     A6
	
	MOVEA.L  old_InitFonts,A0
	JMP      (A0)
}

int main()
{
	enum
	{
		_InitFonts = 0xA8FE,
	};
	
	if ( mac::sys::has_v68k() )
	{
		Handle self = Get1Resource( 'INIT', 0 );
		
		DetachResource( self );
		
		old_InitFonts = mac::sys::get_trap_address( _InitFonts );
		
		mac::sys::set_trap_address( (ProcPtr) InitFonts_patch, _InitFonts );
	}
	
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

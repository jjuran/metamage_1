/*
	WorldTuneup.cc
	--------------
	
	World-building Tune-up INIT for Mac OS
	
	Copyright 2026, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	For "worlds" developed with World Builder.
	
*/

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif
#ifndef __SOUND__
#include <Sound.h>
#endif

// mac-glue-utils
#include "mac_glue/Memory.hh"

// mac-sys-utils
#include "mac_sys/trap_address.hh"

// mac-qd-utils
#include "mac_qd/plot_icon_id.hh"

// ShowAnIcon
#include "ShowAnIcon.hh"

// WorldTuneup
#include "playback.hh"
#include "state.hh"


#pragma exceptions off


using mac::glue::GetHandleSize_raw;

using mac::sys::get_trap_address;
using mac::sys::set_trap_address;


enum
{
	_VInstall      = 0xA033,
	_NewHandle     = 0xA122,
	_ResrvMem_A0   = 0xA140,
	_SndNewChannel = 0xA807,
	_GetResource   = 0xA9A0,
	_LoadResource  = 0xA9A2,
	_TEInit        = 0xA9CC,
};

short SysVersion : 0x015a;


static UniversalProcPtr old_TEInit;

static inline
void my_memcpy( void* dst, const void* src, long n )
{
	/*
		Use BlockMove() instead of BlockMoveData(),
		because we're copying into a code resource
		that's already executing.
	*/
	
	BlockMove( src, dst, n );
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

enum
{
	sizeof_allocate = sizeof (UInt16) * 26,
};

struct PerAppTurf : PerAppState
{
	UInt16 alloc[ sizeof_allocate / sizeof (UInt16) ];
};

static inline
PerAppTurf& get_turf( Handle code_1 )
{
	return static_cast< PerAppTurf& >( get_state( code_1 ) );
}

static
asm
OSErr allocate()
{
	LINK     A6,#0
	
	SUBQ.L   #6,SP     // allocate both results
	MOVE.L   #'CODE',-(SP)
	MOVE.W   #1,-(SP)
	_GetResource
	MOVE.L   (SP)+,A1  // 'CODE' id=1
	
	MOVE.L   (A1),A1   // pointer to code
	
	MOVEQ    #0,D0
	ADDQ.L   #4,A1     // &channel
	MOVE.L   A1,-(SP)  // &channel
	MOVE.W   #5,-(SP)  // synth
	MOVE.L   D0,-(SP)  // init
	MOVE.L   D0,-(SP)  // callback
	
	MOVE.L   D0,(A1)+   // channel = NULL
	
	_NewHandle
	
	BNE.S    fail
	
	MOVE.L   A0,(A1)  // buffer = NewHandle( 0 )
	
	_SndNewChannel
	
	MOVE.W   (SP)+,D0
	
fail:
	
	UNLK     A6
	RTS
}

static
pascal
void sound_callback( SndChannelPtr channel, SndCommand* command )
{
	Handle buffer = (Handle) command->param2;
	
	/*
		Is it safe to call HUnlock() on a locked handle
		at interrupt time?  ... Probably?  Let's hope so!
		
		What if MercutioHUnlockPatch is installed?
	*/
	
	HUnlock( buffer );
	
	SoundActive = false;
}

static
asm
void query_sample()
{
	MOVE.L   D3,D0
	_ResrvMem_A0
	MOVE.L   A3,-(A7)
	_LoadResource
}

static
asm
void check_sample()
{
	CLR.L    (A0)
	MOVE.W   #0x0001,4(A0)
	LEA      *+26,A1
	MOVE.L   A1,6(A0)
	MOVE.W   #0x0001,10(A0)
	CLR.W    12(A0)
	_VInstall
}

static
UInt16* find_patch_site( Handle h, Size handle_size )
{
	enum
	{
		query_sample_length =  8,
		check_sample_length = 28,
		
		query_to_check_gap = 0x34,
		query_to_proc_end  = 0x5e,
	};
	
	typedef UInt16* Code;
	
	UInt16* p = (UInt16*) *h;
	UInt16* z = p + (handle_size - query_to_proc_end) / 2;
	
	for ( ;  p <= z;  ++p )
	{
		if ( ! equal_words( p, (Code) &query_sample, query_sample_length / 2 ) )
		{
			continue;
		}
		
		p = (UInt16*) ((Ptr) p + query_sample_length + query_to_check_gap);
		
		if ( ! equal_words( p, (Code) &check_sample, check_sample_length / 2 ) )
		{
			continue;
		}
		
		return p;
	}
	
	return NULL;
}

static
asm
short call_indirect( void* f : __A0 )
{
	JMP      (A0)
}

static
void install_patch( Handle h, Size handle_size )
{
	enum
	{
		check_sample_length = 28,
	};
	
	/*
		If we find the patch site, then we can confidently
		clobber some code at the beginning of the resource
		which has already executed.  We need to do this in
		order to create a sound channel.  And we want to
		make sure allocation succeeded before installing
		the real code patch.
	*/
	
	if ( UInt16* p = find_patch_site( h, handle_size ) )
	{
		PerAppTurf& state = get_turf( h );
		
		my_memcpy( state.alloc, &allocate, sizeof_allocate );
		
		if ( call_indirect( state.alloc ) == noErr )
		{
			state.channel->callBack = &sound_callback;
			
			my_memcpy( p, &playback_patch, check_sample_length );
		}
		
		/*
			The World Builder runtime uses SoundActive
			as a sempahore:  It sets it true when a
			sound is playing and clears it afterward.
			It frequently waits for SoundActive to be
			clear on the assumption that only itself
			would have set it.  If SoundActive isn't
			already false, the runtime will stall.
		*/
		
		SoundActive = false;
	}
}

static
void TEInit_handler()
{
	if ( Get1Resource( 'GCOD', 0 ) )
	{
		/*
			The 'GCOD' resource is "global code".
			The application will need that anyway,
			so don't bother releasing it here.
		*/
		
		Handle h = Get1Resource( 'CODE', 1 );
		
		install_patch( h, GetHandleSize_raw( h ) );
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
	using show_an_icon::advance_location;
	using show_an_icon::draw_X;
	using show_an_icon::get_icon_rect;
	using show_an_icon::temporary_graphics_port;
	
	temporary_graphics_port port;
	
	Rect r;
	
	get_icon_rect( r );
	
	mac::qd::plot_icon_id( r, 128 );
	
	if ( SysVersion >= 0x0700 )
	{
		Handle self = Get1Resource( 'INIT', 0 );
		
		DetachResource( self );
		
		old_TEInit = get_trap_address( _TEInit );
		
		set_trap_address( (ProcPtr) TEInit_patch, _TEInit );
	}
	else
	{
		draw_X( r );
	}
	
	advance_location();
	
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

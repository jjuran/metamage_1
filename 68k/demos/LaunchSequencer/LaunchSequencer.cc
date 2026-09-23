/*
	LaunchSequencer.cc
	------------------
	
	Application launch sequencer for Advanced Mac Substitute
	
	Copyright 2026, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
*/

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif
#ifndef __SEGLOAD__
#include <SegLoad.h>
#endif
#ifndef __TEXTUTILS__
#include <TextUtils.h>
#endif

// mac-glue-utils
#include "mac_glue/Gestalt.hh"


enum
{
	gestaltLoaderTargetName = 'Load',
};

THz   TheZone    : 0x0118;
THz   SysZone    : 0x02A6;
Str15 FinderName : 0x02E0;
Str31 CurApName  : 0x0910;

struct LaunchParams
{
	void* appName;
	short reserved2;
	// ...
};

#if TARGET_CPU_68K

static inline
short asm Launch( void* pb : __A0 )
{
	DC.W     0xA9F2  // _Launch
}

#endif

static Byte* target_name;

static LaunchParams params;

static inline
void my_memcpy( void* dst, const void* src, long n )
{
	BlockMoveData( src, dst, n );
}

static
void launch()
{
	params.appName = target_name;
	
#if TARGET_CPU_68K
	
	Size len = CurApName[ 0 ];
	
	if ( len <= 15 )
	{
		my_memcpy( FinderName, CurApName, 1 + len );
	}
	
	Launch( &params );
	
#endif
}

static
void launch_next_in_sequence()
{
	if ( *target_name <= 255 - 4 )
	{
		Byte* near_end = target_name + 256 - 4;
		
		Handle& h = *(Handle*) near_end;
		
		if ( ! h )
		{
			THz oldZone = TheZone;
			
			TheZone = SysZone;
			
			h = Get1Resource( 'STR#', 128 );
			
			TheZone = oldZone;
			
			if ( ! h )
			{
				return;
			}
			
			DetachResource( h );
		}
		
		UInt16* start = (UInt16*) *h;
		
		UInt16& count = *start++;
		
		if ( count == 0 )
		{
			return;
		}
		
		--count;
		
		StringPtr next = (Byte*) start;
		
		Size name_size = 1 + next[ 0 ];
		
		my_memcpy( target_name, next, name_size );
		
		Munger( h, 2, NULL, name_size, (Ptr) -1, 0 );
		
		launch();
	}
}

static
void launch_app_file()
{
#if TARGET_CPU_68K  &&  ! TARGET_RT_MAC_CFM
	
	short action;
	short count;
	CountAppFiles( &action, &count );
	
	for ( int i = 1;  i <= count;  ++i )
	{
		AppFile file;
		GetAppFiles( i, &file );
		
		if ( file.fType == 'APPL' )
		{
			my_memcpy( target_name, file.fName, 1 + file.fName[ 0 ] );
			
			ClrAppFiles( i );
			
			launch();
		}
	}
	
#endif
}

int main()
{
	using mac::glue::gestalt;
	
	target_name = (Byte*) gestalt( gestaltLoaderTargetName );
	
	if ( target_name )
	{
		/*
			If a debugger makes us ExitToShell(),
			exit instead of relaunching.
		*/
		
		FinderName[ 0 ] = '\0';
		
		launch_next_in_sequence();
	}
	
	launch_app_file();
	
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

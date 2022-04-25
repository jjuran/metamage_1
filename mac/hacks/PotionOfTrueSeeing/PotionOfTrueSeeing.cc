/*
	PotionOfTrueSeeing.cc
	---------------------
	
	Potion of True Seeing INIT for Mac OS
	
	Copyright 2022, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
		When runes of death confuse you
		And it seems there's no way out
		This brew will clear your vision
		So you won't have any doubt.
	
*/

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif
#ifndef __TRAPS__
#include <Traps.h>
#endif


#pragma exceptions off


static UniversalProcPtr old_LoadSeg;
static UniversalProcPtr old_TEInit;

static
void LoadSeg_handler( short segnum : __D0 )
{
	const short target_segnum = 3;
	
	const int query_offset = 0x001b7e;
	
	Handle h;
	
	if ( segnum == target_segnum  &&  (h = GetResource( 'CODE', segnum )) )
	{
		const Size size = GetHandleSize( h );
		
		if ( size < query_offset + sizeof (UInt32) )
		{
			return;
		}
		
		UInt32* p = (UInt32*) (*h + query_offset);
		
		if ( *p == 0x7c006018 )
		{
			/*
				Old:  MOVEQ    #0,D6
				      BRA.S    *+26    // $001b9a
				New:  MOVEQ    #32,D0
				      BRA.S    *+38    // $001ba6
			*/
			
			*p = 0x70206024;
		}
	}
}

static
pascal asm void LoadSeg_patch( short segnum )
{
	LINK     A6,#0
	MOVEM.L  D0-D2/A0-A1,-(SP)
	
	MOVE.W   8(A6),D0
	JSR      LoadSeg_handler
	
	MOVEM.L  (SP)+,D0-D2/A0-A1
	UNLK     A6
	
	MOVE.L   old_LoadSeg,-(SP)
	RTS
}

static
void TEInit_handler()
{
	if ( Handle h = Get1Resource( 0x506fc450, 0 ) )
	{
		ReleaseResource( h );
		
		old_LoadSeg = NGetTrapAddress( _LoadSeg, ToolTrap );
		
		NSetTrapAddress( (UniversalProcPtr) LoadSeg_patch, _LoadSeg, ToolTrap );
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
	Handle self = Get1Resource( 'INIT', 0 );
	
	DetachResource( self );
	
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

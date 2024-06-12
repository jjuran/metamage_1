/*
	set_custom_icon.cc
	------------------
*/

#include "set_custom_icon.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// mac-sys-utils
#include "mac_sys/res_error.hh"

// tictactoe-logic
#include "tictactoe.hh"

// Tic-tac-toe
#include "custom_icons.hh"
#include "Rez/icons.h"
#include "state.hh"


static inline
void inscribe_game_into_icon( const Byte* game, int n, IconRow* icon_rows )
{
	int state = 0;
	
	for ( int i = 0;  i < n;  ++i )
	{
		Byte code = game[ i ];
		
		unsigned index = tictactoe::decode_cell_index( code );
		
		state = inscribe_move_into_icon( index, icon_rows, state );
	}
}

bool set_custom_icon( const Byte* game, int n_moves )
{
	if ( Handle blank_icon = GetResource( 'ICN#', kBlankCustomIconResID ) )
	{
		Handle saved_icon = GetResource( 'ICN#', kCustomIconResource );
		
		if ( saved_icon )
		{
			ChangedResource( saved_icon );
			
			SetHandleSize( saved_icon, 0 );
			
			HandAndHand( blank_icon, saved_icon );
		}
		else
		{
			DetachResource( blank_icon );
			
			saved_icon = blank_icon;
			
			AddResource( saved_icon, 'ICN#', kCustomIconResource, NULL );
			
			if ( mac::sys::res_error() )
			{
				DisposeHandle( blank_icon );
				
				return false;
			}
		}
		
		IconRow* icon_rows = (IconRow*) *saved_icon;
		
		inscribe_game_into_icon( game, n_moves, icon_rows );
		
		return true;
	}
	
	return false;
}

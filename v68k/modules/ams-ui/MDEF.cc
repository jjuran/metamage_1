/*
	MDEF.cc
	-------
*/

#include "MDEF.hh"

// Mac OS
#ifndef __MENUS__
#include <Menus.h>
#endif


const short menu_item_height = 16;

const short left_padding = 14;
const short right_padding = 6;

static
void MDEF_0_Size( MenuRef menu )
{
	short height = 0;
	
	short maxTextWidth = 0;
	
	menu_item_const_iterator it( menu );
	
	while ( const unsigned char* text = it )
	{
		++it;
		
		height += menu_item_height;
		
		const short textWidth = StringWidth( text );
		
		if ( textWidth > maxTextWidth )
		{
			maxTextWidth = textWidth;
		}
	}
	
	short width = 0;
	
	if ( maxTextWidth > 0 )
	{
		width = left_padding + maxTextWidth + right_padding;
	}
	
	menu[0]->menuWidth  = width;
	menu[0]->menuHeight = height;
}

void MDEF_0( short msg, MenuRef menu, const Rect* r, Point hit, short* which )
{
	switch ( msg )
	{
		case mDrawMsg:
		case mChooseMsg:
			break;
		
		case mSizeMsg:
			MDEF_0_Size( menu );
			break;
		
		default:
			break;
	}
}

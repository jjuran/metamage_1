/*
	MDEF.cc
	-------
*/

#include "MDEF.hh"

// Mac OS
#ifndef __MENUS__
#include <Menus.h>
#endif

// ams-common
#include "QDGlobals.hh"


const short menu_item_height = 16;

const short system_font_ascent =  9;
const short pen_v_offset_for_text = (menu_item_height + system_font_ascent) / 2;
const short pen_v_offset_for_rule = menu_item_height / 2;

const short mark_padding = 2;
const short left_padding = 14;
const short right_padding = 6;

static
void MDEF_0_Draw( MenuRef menu, const Rect& r )
{
	QDGlobals& qd = get_QDGlobals();
	
	const short top = r.top;
	const short left = r.left;
	const short right = r.right;
	
	short v = top + pen_v_offset_for_text - menu_item_height;
	
	menu_item_iterator it( menu );
	
	while ( const unsigned char* text = it )
	{
		++it;
		
		v += menu_item_height;
		
		if ( text[ 0 ] == 1  &&  text[ 1 ] == '-' )
		{
			short lineTop = v - pen_v_offset_for_text + pen_v_offset_for_rule;
			
			Rect line = { lineTop, left, lineTop + 1, right };
			
			FillRect( &line, &qd.gray );
		}
		else
		{
			const unsigned char* p = text + 1 + text[ 0 ];
			
			const uint8_t icon  = *p++;
			const uint8_t key   = *p++;
			const uint8_t mark  = *p++;
			const uint8_t style = *p++;
			
			if ( mark )
			{
				MoveTo( left + mark_padding, v );
				
				DrawChar( mark );
			}
			
			MoveTo( left + left_padding, v );
			
			DrawString( text );
		}
	}
}

static
void invert_item( Rect rect, short item )
{
	if ( item )
	{
		rect.top   += --item   * menu_item_height;
		rect.bottom = rect.top + menu_item_height;
		
		InvertRect( &rect );
	}
}

static
void MDEF_0_Choose( MenuRef menu, const Rect& rect, Point hit, short* which )
{
	short enabledItem = 0;
	
	if ( PtInRect( hit, &rect )  &&  menu[0]->enableFlags & 1 )
	{
		const short dv = hit.v - rect.top;
		
		const short item = dv / menu_item_height + 1;
		
		const short enabled = item > 31  ||  menu[0]->enableFlags & (1 << item);
		
		enabledItem = enabled ? item : 0;
		
		if ( enabledItem == *which )
		{
			return;
		}
	}
	
	invert_item( rect, *which      );
	invert_item( rect, enabledItem );
	
	*which = enabledItem;
}

static
void MDEF_0_Size( MenuRef menu )
{
	short height = 0;
	
	short maxTextWidth = 0;
	
	menu_item_iterator it( menu );
	
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
			MDEF_0_Draw( menu, *r );
			break;
		
		case mChooseMsg:
			MDEF_0_Choose( menu, *r, hit, which );
			break;
		
		case mSizeMsg:
			MDEF_0_Size( menu );
			break;
		
		default:
			break;
	}
}

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

const short key_padding = 1;
const short mark_padding = 2;
const short left_padding = 14;
const short keystroke_gap = 14;
const short right_padding = 6;

static
short keystroke_pen_offset_once()
{
	const uint8_t keystroke[] = { 2, kCommandCharCode, 'M' };
	
	const short width = StringWidth( keystroke );
	
	return width + key_padding;
}

static
short keystroke_pen_offset()
{
	static const short offset = keystroke_pen_offset_once();
	
	return offset;
}

static inline
short total_keystroke_offset()
{
	return keystroke_gap + keystroke_pen_offset();
}

static inline
bool should_draw_key( uint8_t key )
{
	return key > 0x1F;
}

static
void MDEF_0_Draw( MenuRef menu, const Rect& r )
{
	QDGlobals& qd = get_QDGlobals();
	
	const short top = r.top;
	const short left = r.left;
	const short right = r.right;
	
	UInt32 enableFlags = menu[0]->enableFlags;
	
	const bool menuEnabled = enableFlags & 1;
	
	if ( ~enableFlags )
	{
		PenPat( &qd.gray );
		PenMode( patBic );
	}
	
	short v = top + pen_v_offset_for_text - menu_item_height;
	
	menu_item_iterator it( menu );
	
	while ( const unsigned char* text = it )
	{
		++it;
		enableFlags >>= 1;
		
		v += menu_item_height;
		
		if ( text[ 1 ] == '-' )
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
			
			if ( mark  &&  key != hMenuCmd )
			{
				MoveTo( left + mark_padding, v );
				
				DrawChar( mark );
			}
			
			MoveTo( left + left_padding, v );
			
			DrawString( text );
			
			if ( should_draw_key( key ) )
			{
				MoveTo( right - keystroke_pen_offset(), v );
				
				const uint8_t keystroke[] = { 2, kCommandCharCode, key };
				
				DrawString( keystroke );
			}
			
			const bool enabled = enableFlags & 1  &&  menuEnabled;
			
			if ( ! enabled )
			{
				const short top    = v - pen_v_offset_for_text;
				const short bottom = top + menu_item_height;
				
				Rect itemRect = { top, left, bottom, right };
				
				PaintRect( &itemRect );
			}
		}
	}
	
	if ( ~menu[0]->enableFlags )
	{
		PenNormal();
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
	
	short maxItemWidth = 0;  // Excludes mark area, includes right padding
	
	menu_item_iterator it( menu );
	
	while ( const unsigned char* text = it )
	{
		++it;
		
		height += menu_item_height;
		
		const short textWidth = StringWidth( text );
		
		short itemWidth = textWidth + right_padding;
		
		const unsigned char* p = text + 1 + text[ 0 ] + 1;  // skip icon
		
		if ( should_draw_key( *p ) )
		{
			itemWidth += total_keystroke_offset() - right_padding;
		}
		
		if ( itemWidth > maxItemWidth )
		{
			maxItemWidth = itemWidth;
		}
	}
	
	short width = 0;
	
	if ( maxItemWidth > 0 )
	{
		width = left_padding + maxItemWidth;
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

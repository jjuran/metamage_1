/*
	menus.cc
	--------
*/

#include "aglow/menus.hh"

// amicus
#include "amicus/menu_defs.hh"


#define SET_UP_MENU(name)  (name = set_up_menu( name##_desc ))


MenuRef Apple;
MenuRef File;
MenuRef Edit;
MenuRef View;

static
MenuRef set_up_menu( const menu_description& desc )
{
	static int nth = 0;
	
	MenuRef menu = NewMenu( ++nth, desc.title );
	
	InsertMenu( menu, 0 );
	
	for ( short i = 0;  i < desc.count; )
	{
		const menu_item_description& item = desc.items[ i++ ];
		
		ConstStr255Param format = item.text ? item.text : "\p" "-";
		
		AppendMenu( menu, format );
		
		if ( item.command )
		{
			SetMenuItemCommandID( menu, i, item.command );
		}
	}
	
	return menu;
}

void set_up_menus()
{
	SET_UP_MENU( Apple );
	SET_UP_MENU( File  );
	SET_UP_MENU( Edit  );
	SET_UP_MENU( View  );
}

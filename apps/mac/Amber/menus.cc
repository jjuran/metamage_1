/*
	menus.cc
	--------
*/

#include "menus.hh"


#define LENGTH( array )  (sizeof (array) / sizeof *(array))

#define DEFINE_MENU_WITH_TITLE(name, title)  \
static const menu_description name##_desc =  \
{ "\p" title, name##_items, LENGTH( name##_items ) }

#define DEFINE_MENU(title)  DEFINE_MENU_WITH_TITLE(title, #title)

#define SET_UP_MENU(name)  (name = set_up_menu( name##_desc ))


MenuRef Apple;
MenuRef File;
MenuRef Edit;
MenuRef View;

struct menu_item_description
{
	const unsigned char*  format;
	MenuCommand           command;
};

struct menu_description
{
	const unsigned char*          title;
	const menu_item_description*  items;
	short                         count;
};

static
MenuRef set_up_menu( const menu_description& desc )
{
	static int nth = 0;
	
	MenuRef menu = NewMenu( ++nth, desc.title );
	
	InsertMenu( menu, 0 );
	
	for ( short i = 0;  i < desc.count; )
	{
		const menu_item_description& item = desc.items[ i++ ];
		
		ConstStr255Param format = item.format ? item.format : "\p" "-";
		
		AppendMenu( menu, format );
		
		if ( item.command )
		{
			SetMenuItemCommandID( menu, i, item.command );
		}
	}
	
	return menu;
}

static const menu_item_description Apple_items[] =
{
	{ "\p" "About Advanced Mac Substitute", kHICommandAbout },
};

static const menu_item_description File_items[] =
{
	{ "\p" "Close/W", kHICommandClose },
};

static const menu_item_description Edit_items[] =
{
	{ "\p" "Undo/Z",  kHICommandUndo  },
	{},
	{ "\p" "Cut/X",   kHICommandCut   },
	{ "\p" "Copy/C",  kHICommandCopy  },
	{ "\p" "Paste/V", kHICommandPaste },
	{ "\p" "Clear",   kHICommandClear },
};

static const menu_item_description View_items[] =
{
	{ "\p"  "50%", kZoom50Percent  },
	{ "\p" "100%", kZoom100Percent },
	{ "\p" "150%", kZoom150Percent },
	{ "\p" "200%", kZoom200Percent },
	{ "\p" "300%", kZoom300Percent },
};

DEFINE_MENU_WITH_TITLE( Apple, "\x14" );

DEFINE_MENU( File );
DEFINE_MENU( Edit );
DEFINE_MENU( View );

void set_up_menus()
{
	SET_UP_MENU( Apple );
	SET_UP_MENU( File  );
	SET_UP_MENU( Edit  );
	SET_UP_MENU( View  );
}

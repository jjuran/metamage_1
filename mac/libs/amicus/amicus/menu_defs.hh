/*
	menu_defs.hh
	------------
*/

#ifndef AMICUS_MENUDEFS_HH
#define AMICUS_MENUDEFS_HH

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// amicus
#include "amicus/menu_cmds.h"


#define COUNT( array )  (sizeof (array) / sizeof *(array))

#define DEFINE_MENU_WITH_TITLE(name, title)  \
static const menu_description name##_desc =  \
{ STR title, name##_items, COUNT( name##_items ) }

#define DEFINE_MENU(title)  DEFINE_MENU_WITH_TITLE(title, #title)


typedef unsigned char Byte;

#ifdef __OBJC__
#define STR @
typedef NSString* String;
#else
#define STR "\p"
typedef const Byte* String;
#endif

/*
	The text field must contain only plain text,
	with no style/icon/key information.  This is
	because the same text will be interpreted
	both by the Menu Manager (which will parse
	and recognize such formatting) and by Cocoa,
	which will use the given text verbatim.
*/

struct menu_item_description
{
	String       text;
	MenuCommand  command;
};

struct menu_description
{
	String                        title;
	const menu_item_description*  items;
	short                         count;
};

static const menu_item_description Apple_items[] =
{
	{ STR "About Advanced Mac Substitute", kHICommandAbout },
#ifdef __OBJC__
	{},
	{ STR "Quit Advanced Mac Substitute", kHICommandQuit },
#endif
};

static const menu_item_description File_items[] =
{
	{ STR "Close", kHICommandClose },
};

static const menu_item_description Edit_items[] =
{
	{ STR "Undo",  kHICommandUndo  },
	{},
	{ STR "Cut",   kHICommandCut   },
	{ STR "Copy",  kHICommandCopy  },
	{ STR "Paste", kHICommandPaste },
	{ STR "Clear", kHICommandClear },
};

static const menu_item_description View_items[] =
{
	{ STR  "50%", kZoom50Percent  },
	{ STR "100%", kZoom100Percent },
	{ STR "150%", kZoom150Percent },
	{ STR "200%", kZoom200Percent },
	{ STR "250%", kZoom250Percent },
	{ STR "300%", kZoom300Percent },
	{ STR "350%", kZoom350Percent },
	{ STR "400%", kZoom400Percent },
};

DEFINE_MENU_WITH_TITLE( Apple, "\x14" );

DEFINE_MENU( File );
DEFINE_MENU( Edit );
DEFINE_MENU( View );

#endif

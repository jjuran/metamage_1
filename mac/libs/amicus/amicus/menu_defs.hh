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
{ "\p" title, name##_items, COUNT( name##_items ) }

#define DEFINE_MENU(title)  DEFINE_MENU_WITH_TITLE(title, #title)


typedef unsigned char Byte;

struct menu_item_description
{
	const Byte*  format;
	MenuCommand  command;
};

struct menu_description
{
	const Byte*                   title;
	const menu_item_description*  items;
	short                         count;
};

static const menu_item_description Apple_items[] =
{
	{ "\p" "About Advanced Mac Substitute", kHICommandAbout },
};

static const menu_item_description File_items[] =
{
	{ "\p" "Close", kHICommandClose },
};

static const menu_item_description Edit_items[] =
{
	{ "\p" "Undo",  kHICommandUndo  },
	{},
	{ "\p" "Cut",   kHICommandCut   },
	{ "\p" "Copy",  kHICommandCopy  },
	{ "\p" "Paste", kHICommandPaste },
	{ "\p" "Clear", kHICommandClear },
};

static const menu_item_description View_items[] =
{
	{ "\p"  "50%", kZoom50Percent  },
	{ "\p" "100%", kZoom100Percent },
	{ "\p" "150%", kZoom150Percent },
	{ "\p" "200%", kZoom200Percent },
	{ "\p" "250%", kZoom250Percent },
	{ "\p" "300%", kZoom300Percent },
	{ "\p" "350%", kZoom350Percent },
	{ "\p" "400%", kZoom400Percent },
};

DEFINE_MENU_WITH_TITLE( Apple, "\x14" );

DEFINE_MENU( File );
DEFINE_MENU( Edit );
DEFINE_MENU( View );

#endif

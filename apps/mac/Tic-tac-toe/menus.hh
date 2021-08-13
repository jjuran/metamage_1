/*
	menus.hh
	--------
*/

#ifndef TICTACTOE_MENUS_HH
#define TICTACTOE_MENUS_HH

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __MENUS__
#include <Menus.h>
#endif

enum
{
	Apple = 1,
	File,
	Edit,
	Options,
	
	Apple_menu_items = 0,
	About,
	
	File_menu_items = 0,
	NewGame,
	Open,
	Close,
	Save,
	SaveAs,
	File_divider_before_Quit,
	Quit,
	
	Edit_menu_items = 0,
	Undo,
	
	Options_menu_items = 0,
	Sound,
	Fullscreen,
};

extern MenuRef Edit_menu;
extern MenuRef Options_menu;

void set_up_menus();

#endif

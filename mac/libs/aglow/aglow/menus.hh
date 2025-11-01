/*
	menus.hh
	--------
*/

#ifndef AGLOW_MENUS_HH
#define AGLOW_MENUS_HH

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif


extern MenuRef Apple;
extern MenuRef File;
extern MenuRef Edit;
extern MenuRef View;

void set_up_menus();

#endif

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


enum
{
	kZoom50Percent  = ' 50%',
	kZoom100Percent = '100%',
	kZoom150Percent = '150%',
	kZoom200Percent = '200%',
	kZoom250Percent = '250%',
	kZoom300Percent = '300%',
	kZoom350Percent = '350%',
	kZoom400Percent = '400%',
};

extern MenuRef Apple;
extern MenuRef File;
extern MenuRef Edit;
extern MenuRef View;

void set_up_menus();

#endif

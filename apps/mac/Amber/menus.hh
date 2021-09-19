/*
	menus.hh
	--------
*/

#ifndef MENUS_HH
#define MENUS_HH

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
	kZoom300Percent = '300%',
};

extern MenuRef Apple;
extern MenuRef File;
extern MenuRef Edit;
extern MenuRef View;

void set_up_menus();

#endif

/*
	UI.hh
	-----
*/

#ifndef UI_HH
#define UI_HH

#ifndef __CONTROLS__
#include <Controls.h>
#endif


enum
{
	Apple = 1,
	File  = 2,
	
	About = 1,
	
	Install = 1,
	Close   = 2,
	// ---- = 3,
	Quit    = 4,
};

extern WindowRef  console_window;
extern ControlRef install_button;

extern OSErr install_error;

extern bool installed;

void install();
void quit();

void default_button_click();

#endif

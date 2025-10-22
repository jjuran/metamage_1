/*
	UI.hh
	-----
*/

#ifndef UI_HH
#define UI_HH


struct ControlRecord;
struct GrafPort;

typedef unsigned char Byte;

typedef short OSErr;

typedef ControlRecord** ControlRef;
typedef GrafPort*       WindowRef;


enum
{
	Apple = 1,
	File  = 2,
	
	About = 1,
	
	Test    = 1,
	Stop    = 2,
	Close   = 3,
	// ---- = 4,
	Quit    = 5,
};

extern const Byte play_button_name[];
extern const Byte stop_button_name[];

extern WindowRef  console_window;
extern ControlRef play_stop_button;

extern OSErr write_error;

void play();
void stop();

void default_button_click();

#endif

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
	
	Start   = 1,
	Stop    = 2,
	Close   = 3,
	// ---- = 4,
	Quit    = 5,
};

extern const Byte start_button_name[];
extern const Byte stop_button_name[];

extern WindowRef  console_window;
extern ControlRef start_stop_button;

void startup();
void cleanup();

#endif

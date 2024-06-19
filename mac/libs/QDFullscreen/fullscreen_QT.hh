/*
	fullscreen_QT.hh
	----------------
*/

#ifndef FULLSCREENQT_HH
#define FULLSCREENQT_HH


extern char* fullscreen_context;

inline
bool is_fullscreen_via_QT()
{
	return fullscreen_context != 0;  // NULL
}

void begin_fullscreen();
void end_fullscreen();

#endif

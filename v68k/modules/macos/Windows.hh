/*
	Windows.hh
	----------
*/

#ifndef WINDOWS_HH
#define WINDOWS_HH

pascal void InitWindows_patch();

pascal short FindWindow_patch( struct Point pt, struct GrafPort** window );

#endif

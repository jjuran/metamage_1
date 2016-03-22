/*
	Windows.hh
	----------
*/

#ifndef WINDOWS_HH
#define WINDOWS_HH

extern struct MacRegion** BezelRgn;

pascal void ClipAbove_patch( struct WindowRecord* window );

pascal void PaintOne_patch   ( struct WindowRecord* w, struct MacRegion** rgn );
pascal void PaintBehind_patch( struct WindowRecord* w, struct MacRegion** rgn );

pascal void InitWindows_patch();

pascal short FindWindow_patch( struct Point pt, struct GrafPort** window );

#endif

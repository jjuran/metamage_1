/*
	Magic.hh
	--------
*/

#ifndef MAGIC_HH
#define MAGIC_HH

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __MACWINDOWS__
#include <MacWindows.h>
#endif


extern Rect last_menu_draw_rect;

#if CALL_NOT_IN_CARBON

long MenuSelect_magic( Point pt );

Boolean TrackGoAway_magic( WindowRef window, Point pt );

void DragWindow_magic( WindowRef    window,
                       Point        pt,
                       const Rect*  area );

#endif  // #if CALL_NOT_IN_CARBON

#endif

/*
	cursor_jump.hh
	--------------
*/

#ifndef CURSORJUMP_HH
#define CURSORJUMP_HH


typedef void (*CursorProcPtr)();  // pascal and C have same calling sequence

CursorProcPtr JHideCursor : 0x0800;
CursorProcPtr JShowCursor : 0x0804;

#endif

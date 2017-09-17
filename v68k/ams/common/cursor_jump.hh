/*
	cursor_jump.hh
	--------------
*/

#ifndef CURSORJUMP_HH
#define CURSORJUMP_HH

struct Cursor;

typedef void (*CursorProcPtr)();  // pascal and C have same calling sequence

typedef pascal void (*SetCursorProcPtr)( const Cursor* crsr );

CursorProcPtr JHideCursor : 0x0800;
CursorProcPtr JShowCursor : 0x0804;
CursorProcPtr JInitCrsr   : 0x0814;
SetCursorProcPtr JSetCrsr : 0x0818;

#endif

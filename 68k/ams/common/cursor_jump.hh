/*
	cursor_jump.hh
	--------------
*/

#ifndef CURSORJUMP_HH
#define CURSORJUMP_HH

struct Cursor;
struct Point;
struct Rect;

typedef void (*CursorProcPtr)();  // pascal and C have same calling sequence

typedef pascal void (*ShieldCursorProcPtr)( const Rect* r, Point pt );
typedef pascal void (*SetCursorProcPtr)( const Cursor* crsr );

CursorProcPtr       JHideCursor   : 0x0800;
CursorProcPtr       JShowCursor   : 0x0804;
ShieldCursorProcPtr JShieldCursor : 0x0808;
CursorProcPtr       JInitCrsr     : 0x0814;
SetCursorProcPtr    JSetCrsr      : 0x0818;
CursorProcPtr       JCrsrObscure  : 0x081C;

#endif

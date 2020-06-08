/*
	Cursor.hh
	---------
*/

#ifndef CURSOR_HH
#define CURSOR_HH

struct Cursor;
struct Point;
struct Rect;

pascal void InitCursor_patch();

pascal void SetCursor_patch( const Cursor* crsr );

pascal void HideCursor_patch();
pascal void ShowCursor_patch();

pascal void ObscureCursor_patch();

/*
	ShieldCursor() is actually documented in Toolbox Utilities, but speaking
	functionally as well as trap-numerically, it belongs here.
*/

pascal void ShieldCursor_patch( const Rect* rect, Point offset );

#endif

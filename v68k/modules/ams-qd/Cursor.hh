/*
	Cursor.hh
	---------
*/

#ifndef CURSOR_HH
#define CURSOR_HH

struct Cursor;

pascal void InitCursor_patch();

pascal void SetCursor_patch( const Cursor* crsr );

pascal void HideCursor_patch();
pascal void ShowCursor_patch();

#endif

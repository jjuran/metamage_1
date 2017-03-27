/*
	Cursor.hh
	---------
*/

#ifndef CURSOR_HH
#define CURSOR_HH

struct Cursor;

void hide_cursor();
void show_cursor();

void update_cursor_location();

pascal void InitCursor_patch();

pascal void SetCursor_patch( const Cursor* crsr );

pascal void HideCursor_patch();
pascal void ShowCursor_patch();

#endif

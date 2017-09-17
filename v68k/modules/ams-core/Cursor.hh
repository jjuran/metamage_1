/*
	Cursor.hh
	---------
*/

#ifndef CURSOR_HH
#define CURSOR_HH

struct Cursor;

void init_lowmem_Cursor();

void init_cursor();

pascal void set_cursor( const Cursor* crsr );

void hide_cursor();
void show_cursor();

void update_cursor_location();

pascal void InitCursor_patch();

pascal void SetCursor_patch( const Cursor* crsr );

pascal void HideCursor_patch();
pascal void ShowCursor_patch();

#endif

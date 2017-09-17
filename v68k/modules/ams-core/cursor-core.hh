/*
	cursor-core.hh
	--------------
*/

#ifndef CURSORCORE_HH
#define CURSORCORE_HH

struct Cursor;

void init_lowmem_Cursor();

void init_cursor();

pascal void set_cursor( const Cursor* crsr );

void hide_cursor();
void show_cursor();

void update_cursor_location();

#endif

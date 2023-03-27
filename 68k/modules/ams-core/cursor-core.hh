/*
	cursor-core.hh
	--------------
*/

#ifndef CURSORCORE_HH
#define CURSORCORE_HH

struct Cursor;
struct Point;
struct Rect;

void init_lowmem_Cursor();

void init_cursor();

pascal void set_cursor( const Cursor* crsr );

void hide_cursor();
void show_cursor();

pascal void shield_cursor( const Rect* r, Point pt );

void update_cursor_location();

void obscure_cursor();

#endif

/*
	Cursor.hh
	---------
*/

#ifndef CURSOR_HH
#define CURSOR_HH

void hide_cursor();
void show_cursor();

void update_cursor_location();

class cursor_lock
{
	private:
		// non-copyable
		cursor_lock           ( const cursor_lock& );
		cursor_lock& operator=( const cursor_lock& );
	
	public:
		cursor_lock()
		{
			hide_cursor();
		}
		
		~cursor_lock()
		{
			show_cursor();
		}
};

pascal void InitCursor_patch();

pascal void HideCursor_patch();
pascal void ShowCursor_patch();

#endif

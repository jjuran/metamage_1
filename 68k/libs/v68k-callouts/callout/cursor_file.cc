/*
	cursor_file.cc
	--------------
*/

#include "cursor_file.hh"

// Standard C
#include <errno.h>
#include <string.h>

// v68k-cursor
#include "cursor/cursor.hh"

// v68k-callouts
#include "callout/cursor_state.hh"

// v68k-screen
#include "screen/shared_memory.hh"


namespace v68k    {
namespace callout {

int set_cursor_backing_store_file( const char* path )
{
	using v68k::screen::open_shared_memory;
	
	void* alloc = open_shared_memory( path );
	
	if ( alloc == 0 )  // NULL
	{
		return errno;
	}
	
	cursor_state = (v68k::cursor::shared_cursor_state*) alloc;
	
	if ( cursor_state )
	{
		memset( cursor_state, '\0', sizeof *cursor_state );
	}
	
	return 0;
}

}  // namespace callout
}  // namespace v68k

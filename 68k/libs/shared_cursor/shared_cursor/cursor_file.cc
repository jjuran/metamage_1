/*
	cursor_file.cc
	--------------
*/

#include "shared_cursor/cursor_file.hh"

// POSIX
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>


using v68k::cursor::shared_cursor_state;

const shared_cursor_state* open_cursor_file( const char* path )
{
	const shared_cursor_state* cursor_state = NULL;
	
	int fd = open( path, O_RDONLY );
	
	if ( fd >= 0 )
	{
		struct stat st;
		
		int nok = fstat( fd, &st );
		
		if ( ! nok )
		{
			void* p = mmap( NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0 );
			
			cursor_state = (const shared_cursor_state*) p;
		}
		
		close( fd );
	}
	
	return cursor_state;
}

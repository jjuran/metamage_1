/*
	copier.cc
	---------
	
	Copyright 2010, Joshua Juran
*/

// Standard C++
#include <algorithm>

// iota
#include "iota/strings.hh"

// Lamp
#include "lamp/copyfile.h"

// text-input
#include "text_input/feed.hh"
#include "text_input/get_line_from_feed.hh"

// poseven
#include "poseven/extras/fd_reader.hh"
#include "poseven/functions/mkdir.hh"
#include "poseven/functions/write.hh"


namespace p7 = poseven;


// Copy a file:
// source-path NUL dest-path NUL LF

// Create a directory:
// NUL dest-path NUL LF


int main( int argc, char const *const argv[] )
{
	text_input::feed feed;
	
	p7::fd_reader reader( p7::stdin_fileno );
	
	while ( const std::string* s = get_line_from_feed( feed, reader ) )
	{
		const std::string& line = *s;
		
		const char* src =       line.data();
		const char* end = src + line.size();
		
		const char* separator = std::find( src, end, '\0' );
		
		if ( separator == end )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "copier: malformed line\n" ) );
			
			return 1;
		}
		
		const char* dest = separator + 1;
		
		separator = std::find( dest, end, '\0' );
		
		if ( separator + 2 != end )
		{
			p7::write( p7::stderr_fileno, STR_LEN( "copier: malformed line\n" ) );
			
			return 1;
		}
		
		if ( *src == '\0' )
		{
			p7::mkdir( dest );
		}
		else
		{
			p7::throw_errno( copyfile( src, dest ) );
		}
	}
	
	return 0;
}


/*
	text_input/feed.cc
	------------------
*/

#include "text_input/feed.hh"

// more-libc
#include "more/string.h"

// gear
#include "gear/find.hh"

// debug
#include "debug/assert.hh"


#ifndef NULL
#define NULL  0L
#endif


namespace text_input
{
	
	void feed::advance_CRLF()
	{
		if ( its_last_end_was_CR  &&  its_mark < its_data_length  &&  its_buffer[ its_mark ] == '\n' )
		{
			++its_mark;
			
			its_last_end_was_CR = false;
		}
	}
	
	const plus::string* feed::get_line_bare()
	{
		const char* begin = &its_buffer[ its_mark        ];
		const char* end   = &its_buffer[ its_data_length ];
		
		ASSERT( begin <= end );
		
		const unsigned char newlines[] = { 2, '\n', '\r' };
		
		const char* eol = gear::find_first_match( begin, end, newlines );
		
		if ( eol != NULL )
		{
			its_last_line.assign( begin, eol );
			
			its_last_end_was_CR = *eol++ == '\r';
			
			its_mark += eol - begin;
			
			advance_CRLF();
			
			if ( !its_next_line.empty() )
			{
				its_next_line += its_last_line;
				
				its_last_line = its_next_line.move();
				
				its_next_line.clear();
			}
			
			return &its_last_line;
		}
		
		its_next_line.append( begin, end );
		
		its_mark = its_data_length;
		
		return NULL;
	}
	
	const plus::string* feed::get_line()
	{
		if ( const plus::string* line = get_line_bare() )
		{
			plus::var_string temp = its_last_line.move();
			
			temp += '\n';
			
			its_last_line = temp.move();
			
			return &its_last_line;
		}
		
		return NULL;
	}
	
	const plus::string& feed::get_fragment_ref()
	{
		const char* begin = &its_buffer[ its_mark        ];
		const char* end   = &its_buffer[ its_data_length ];
		
		its_next_line.append( begin, end );
		
		its_mark = its_data_length;
		
		its_last_line = its_next_line.move();
		
		its_next_line.clear();
		
		return its_last_line;
	}
	
	const plus::string* feed::get_fragment()
	{
		(void) get_fragment_ref();
		
		return its_last_line.empty() ? NULL : &its_last_line;
	}
	
	char* feed::buffer()
	{
		if ( its_mark != its_data_length )
		{
			ASSERT( its_mark < its_data_length );
			
			throw buffer_occupied();
		}
		
		return its_buffer;
	}
	
	void feed::accept_input( size_type length )
	{
		ASSERT( length <= buffer_length );
		
		ASSERT( its_mark == its_data_length );
		
		its_data_length = length;
		its_mark        = 0;
		
		advance_CRLF();
	}
	
	void feed::accept_input( const char* buffer, size_type length )
	{
		if ( length > buffer_length )
		{
			throw buffer_overrun();
		}
		
		mempcpy( this->buffer(), buffer, length );
		
		accept_input( length );
	}
	
}

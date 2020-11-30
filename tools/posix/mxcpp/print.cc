/*
	print.cc
	--------
*/

#include "print.hh"

// Standard C
#include <stdlib.h>
#include <string.h>

// plus
#include "plus/string.hh"

// poseven
#include "poseven/functions/write.hh"


namespace tool
{
	
	namespace p7 = poseven;
	
	
	static inline
	size_t min( size_t a, size_t b )
	{
		return b < a ? b : a;
	}
	
	static void finish_printing()
	{
		print( NULL, 0, true );
	}
	
	void print( const char* line, size_t length, bool flush )
	{
		static char output_buffer[ 4096 ];
		
		static size_t buffer_mark = 0;
		
		static int atexit_result = atexit( &finish_printing ); 
		
		size_t line_mark = 0;
		
		while ( line_mark < length )
		{
			const size_t n = min( length - line_mark, sizeof output_buffer - buffer_mark );
			
			memcpy( &output_buffer[ buffer_mark ], &line[ line_mark ], n );
			
			buffer_mark += n;
			line_mark   += n;
			
			if ( buffer_mark == sizeof output_buffer )
			{
				p7::write( p7::stdout_fileno, output_buffer, sizeof output_buffer );
				
				buffer_mark = 0;
			}
		}
		
		if ( flush )
		{
			p7::write( p7::stdout_fileno, output_buffer, buffer_mark );
			
			buffer_mark = 0;
		}
	}
	
	void print( const plus::string& line, bool flush )
	{
		print( line.data(), line.size() );
	}
	
}

/*
	settings.cc
	-----------
*/

#include "frend/settings.hh"

// Standard C
#include <string.h>

// gear
#include "gear/parse_decimal.hh"

// plus
#include "plus/string.hh"

// frontend-common
#include "frend/cursor.hh"
#include "frend/load_file.hh"


namespace frend
{

static
void parse_settings( const char* text )
{
	const char* p = text;
	
	while ( const char* q = strchr( p, '\n' ) )
	{
		if ( const char* colon = strchr( p, ':' ) )
		{
			const char* value = colon + 1;
			
			size_t n = value - p;
			
			while ( *value == ' ' )
			{
				++value;
			}
			
			unsigned x = gear::parse_unsigned_decimal( value );
			
			if ( strncmp( p, "hide-cursor:", n ) == 0 )
			{
				cursor_to_be_pinned = x;
			}
		}
		
		p = q + 1;
	}
}

void load_settings( int dir_fd, const char* path )
{
	parse_settings( load_file( dir_fd, path ).c_str() );
}

}

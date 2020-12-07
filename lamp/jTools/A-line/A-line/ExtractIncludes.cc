/*	==================
 *	ExtractIncludes.cc
 *	==================
 */

#include "A-line/ExtractIncludes.hh"

// Standard C
#include <string.h>

// Iota
#include "iota/strings.hh"

// text-input
#include "text_input/feed.hh"
#include "text_input/get_line_from_feed.hh"

// poseven
#include "poseven/extras/fd_reader.hh"
#include "poseven/functions/open.hh"
#include "poseven/functions/write.hh"

// A-line
#include "A-line/Includes.hh"


namespace tool
{
	
	namespace n = nucleus;
	namespace p7 = poseven;
	
	
	static void ExtractInclude( const plus::string& line, IncludesCache& includes )
	{
		struct BadIncludeDirective {};
		
		std::size_t pos = line.find_first_not_of( " \t" );
		
		if ( ~pos  &&  line[ pos ] == '#' )
		{
			if ( memcmp( line.data() + pos + 1, STR_LEN( "include" ) ) == 0 )
			{
				try
				{
					pos = line.find_first_not_of( " \t", pos + 1 + STRLEN( "include" ) );
					
					if ( !~pos )  throw BadIncludeDirective();
					
					char c;
					
					if ( line[ pos ] == '"' )
					{
						c = '"';
					}
					else if ( line[ pos ] == '<' )
					{
						c = '>';
					}
					else
					{
						// Not "" or <>, maybe a macro
						//throw BadIncludeDirective();
						return;
					}
					
					++pos;
					std::size_t end = line.find( c, pos );
					
					if ( !~end )  throw BadIncludeDirective();
					
					StringVector& v( c == '"' ? includes.user : includes.system );
					
					v.push_back( line.substr( pos, end - pos ) );
				}
				catch ( const BadIncludeDirective& )
				{
					p7::write( p7::stderr_fileno, STR_LEN( "Bad include!\n" ) );
				}
				catch ( ... )
				{
				}
			}
		}
	}
	
	void ExtractIncludes( IncludesCache& result, const char* pathname )
	{
		text_input::feed feed;
		
		n::owned< p7::fd_t > fd = p7::open( pathname, p7::o_rdonly );
		
		p7::fd_reader reader( fd );
		
		while ( const plus::string* s = get_line_bare_from_feed( feed, reader ) )
		{
			plus::string line( *s );
			
			ExtractInclude( line, result );
		}
	}
	
}

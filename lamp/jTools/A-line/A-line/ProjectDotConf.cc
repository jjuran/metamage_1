/*	=================
 *	ProjectDotConf.cc
 *	=================
 */

#include "A-line/ProjectDotConf.hh"

// text-input
#include "text_input/feed.hh"
#include "text_input/get_line_from_feed.hh"

// poseven
#include "poseven/extras/fd_reader.hh"
#include "poseven/functions/open.hh"

// A-line
#include "A-line/ProjectCatalog.hh"


namespace tool
{
	
	namespace n = nucleus;
	namespace p7 = poseven;
	
	
	static std::pair< plus::string, plus::string > ParseKeyValue( const plus::string& line )
	{
		//if (line.size() == 0)  return;  // Redundant
		const char* ws = " \t";
		std::size_t posWhitespace = line.find_first_of( ws );
		// FIXME:  This will break when leading whitespace precedes key
		plus::string key = line.substr( 0, posWhitespace );
		plus::string value;
		
		if ( ~posWhitespace )
		{
			// Whitespace follows the key
			std::size_t posValue = line.find_first_not_of( ws, posWhitespace );
			
			if ( ~posValue )
			{
				// A value is present
				std::size_t posLastPrintableChar = line.find_last_not_of( ws );
				std::size_t posEndOfValue = posLastPrintableChar + 1;
				value = line.substr( posValue, posEndOfValue );
			}
		}
		
		return std::make_pair( key, value );
	}
	
	static void Tokenize( const plus::string& line,
	                      std::back_insert_iterator< StringVector > tokens )
	{
		typedef std::size_t size_type;
		
		const char* ws = " \t";
		size_type iLastPrintableChar = line.find_last_not_of( ws );
		
		if ( !~iLastPrintableChar )
		{
			return;
		}
		
		size_type endOfData = iLastPrintableChar + 1;
		
		for ( size_type i = 0;  i < endOfData;  )
		{
			size_type tokenStart = line.find_first_not_of( ws, i          );
			size_type tokenEnd   = line.find_first_of    ( ws, tokenStart );
			
			*tokens++ = line.substr( tokenStart, tokenEnd - tokenStart );
			
			i = tokenEnd;
		}
	}
	
	void ReadProjectDotConf( const char* pathname, DotConfData& data )
	{
		text_input::feed feed;
		
		// Open the config file, and read as a text input stream.
		n::owned< p7::fd_t > fd = p7::open( pathname, p7::o_rdonly );
		
		p7::fd_reader reader( fd );
		
		std::size_t lineCount = 0;
		
		while ( const plus::string* s = get_line_bare_from_feed( feed, reader ) )
		{
			plus::string text( *s );
			
			++lineCount;
			// Skip blank lines
			if ( text.size() == 0 )  continue;
			// Skip comments
			if ( text[ 0 ] == '#' )  continue;
			
			data.push_back( DotConfLine() );
			DotConfLine& line( data.back() );
			line.lineNumber = lineCount;
			
			// Parse a line into key and value
			//G::ref( line.key, value ) = ParseKeyValue( text );
			std::pair< plus::string, plus::string > kv = ParseKeyValue( text );
			
			line.key           = kv.first;
			plus::string value = kv.second;
			
			// Check for double-quotes
			std::size_t size = value.size();
			
			if ( value[ 0 ] == '"'  &&  value[ size - 1 ] == '"' )
			{
				// Strip off double-quotes and push single value
				value.assign( &value[ 1 ], size - 2 );
				
				line.values.push_back( value );
			}
			else
			{
				// Copy multiple tokens
				Tokenize( value, std::back_inserter( line.values ) );
			}
		}
	}
	
}

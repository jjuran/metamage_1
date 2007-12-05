/*	==================
 *	ExtractIncludes.cc
 *	==================
 */

#include "A-line/ExtractIncludes.hh"

// Iota
#include "iota/strings.hh"

// POSeven
#include "POSeven/Open.hh"

// Io
#include "Io/TextInput.hh"

// BitsAndBytes
#include "StringPredicates.hh"


namespace ALine
{
	
	namespace NN = Nucleus;
	namespace p7 = poseven;
	
	using BitsAndBytes::eos;
	
	
	static void ExtractInclude( const std::string& line, std::vector< std::string >& includes )
	{
		struct BadIncludeDirective {};
		
		std::size_t pos = line.find_first_not_of( " \t" );
		
		if ( !eos( pos )  &&  line[ pos ] == '#' )
		{
			std::string include = "include";
			
			if ( line.substr( pos + 1, include.size() ) == include )
			{
				try
				{
					pos = line.find_first_not_of( " \t", pos + 1 + include.size() );
					
					if ( eos( pos ) )  throw BadIncludeDirective();
					
					char c;
					
					if ( line[ pos ] == '"' )
					{
						c = '"';
					}
					else if ( line[ pos ] == '<' )
					{
						return;  // Ignore system includes for now
						
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
					
					if ( eos( end ) )  throw BadIncludeDirective();
					
					includes.push_back( line.substr( pos, end - pos ) );
				}
				catch ( const BadIncludeDirective& )
				{
					p7::write( p7::stderr_fileno, STR_LEN( "Bad include!\n" ) );
				}
				catch ( ... )
				{
					p7::write( p7::stderr_fileno, STR_LEN( "Bad compiler!\n" ) );
				}
			}
		}
	}
	
	std::vector< std::string > ExtractIncludes( const std::string& pathname )
	{
		Io::TextInputAdapter< NN::Owned< p7::fd_t > > input( io::open_for_reading( pathname ) );
		
		std::vector< std::string > includes;
		
		while ( input.Ready() )
		{
			std::string line = input.Read();
			
			ExtractInclude( line, includes );
		}
		
		return includes;
	}
	
}


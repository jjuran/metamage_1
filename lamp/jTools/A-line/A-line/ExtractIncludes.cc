/*	==================
 *	ExtractIncludes.cc
 *	==================
 */

#include "A-line/ExtractIncludes.hh"

// Iota
#include "iota/strings.hh"

// poseven
#include "poseven/FileDescriptor.hh"
#include "poseven/functions/open.hh"
#include "poseven/functions/write.hh"

// Io
#include "Io/TextInput.hh"

// BitsAndBytes
#include "StringPredicates.hh"


namespace tool
{
	
	namespace NN = Nucleus;
	namespace p7 = poseven;
	
	using BitsAndBytes::eos;
	
	
	static void ExtractInclude( const std::string& line, IncludesCache& includes )
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
					
					std::vector< std::string >& v( c == '"' ? includes.user : includes.system );
					
					v.push_back( line.substr( pos, end - pos ) );
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
	
	IncludesCache ExtractIncludes( const std::string& pathname )
	{
		Io::TextInputAdapter< NN::Owned< p7::fd_t > > input( io::open_for_reading( pathname ) );
		
		IncludesCache includes;
		
		while ( input.Ready() )
		{
			std::string line = input.Read();
			
			ExtractInclude( line, includes );
		}
		
		return includes;
	}
	
}


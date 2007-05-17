/*	==================
 *	ExtractIncludes.cc
 *	==================
 */

#include "A-line/ExtractIncludes.hh"

// POSeven
#include "POSeven/FileDescriptor.hh"

// Nitrogen Extras / Utilities
#include "Utilities/Files.h"

// Io
#include "Io/TextInput.hh"

// BitsAndBytes
#include "StringPredicates.hh"

// Orion
#include "Orion/StandardIO.hh"


namespace ALine
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace P7 = POSeven;
	
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
					
					pos++;
					std::size_t end = line.find( c, pos );
					
					if ( eos( end ) )  throw BadIncludeDirective();
					
					includes.push_back( line.substr( pos, end - pos ) );
				}
				catch ( BadIncludeDirective )
				{
					Io::Err << "Bad include!\n";
				}
				catch ( ... )
				{
					Io::Err << "Bad compiler!\n";
				}
			}
		}
	}
	
	std::vector< std::string > ExtractIncludes( const FSSpec& file )
	{
		std::string pathname = N::FSpGetPOSIXPathname( file );
		
		Io::TextInputAdapter< NN::Owned< P7::FileDescriptor > > input( io::open_for_reading( pathname ) );
		
		std::vector< std::string > includes;
		
		while ( input.Ready() )
		{
			std::string line = input.Read();
			
			ExtractInclude( line, includes );
		}
		
		return includes;
	}
	
}


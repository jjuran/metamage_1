/*	==================
 *	ExtractIncludes.cc
 *	==================
 */

#include "A-line/ExtractIncludes.hh"

// Io
#include "Io/Files.hh"
#include "Io/MakeHandle.hh"
#include "Io/TextInput.hh"

// BitsAndBytes
#include "StringPredicates.hh"

// Orion
#include "Orion/StandardIO.hh"


namespace ALine
{
	
	namespace N = Nitrogen;
	
	using std::string;
	
	using BitsAndBytes::eos;
	
	
	std::vector< string > ExtractIncludes( const FSSpec& file )
	{
		struct BadIncludeDirective {};
		
		using N::fsRdPerm;
		
		Io::TextInputAdapter input( Io::MakeHandleFromCopy< Io::FileRefNum_Details >( N::FSpOpenDF( file, fsRdPerm ) ) );
		
		std::vector< string > includes;
		
		while ( input.Ready() )
		{
			string line = input.Read();
			string::size_type pos = line.find_first_not_of( " \t" );
			if ( !eos( pos )  &&  line[ pos ] == '#' )
			{
				string include = "include";
				if ( line.substr( pos + 1, include.size() ) == include )
				{
					try
					{
						pos = line.find_first_not_of( " \t", pos + 1 + include.size() );
						if (eos(pos))  throw BadIncludeDirective();
						char c;
						if ( line[ pos ] == '"' )
						{
							c = '"';
						}
						else if ( line[ pos ] == '<' )
						{
							continue;  // Ignore system includes for now
							c = '>';
						}
						else
						{
							// Not "" or <>, maybe a macro
							//throw BadIncludeDirective();
							continue;
						}
						pos++;
						string::size_type end = line.find( c, pos );
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
		
		return includes;
	}
	
}


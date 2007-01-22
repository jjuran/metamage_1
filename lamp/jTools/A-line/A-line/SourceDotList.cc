/*	================
 *	SourceDotList.cc
 *	================
 */

#include "A-line/SourceDotList.hh"

// Io
#include "Io/TextInput.hh"

// BitsAndBytes
#include "StringPredicates.hh"


namespace ALine
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	using std::string;
	
	using BitsAndBytes::eos;
	
	
	std::vector< string > ReadSourceDotList( const FSSpec& sourceList )
	{
		Io::TextInputAdapter< NN::Owned< N::FSFileRefNum > > input( io::open_for_reading( sourceList ) );
		
		std::vector< string > files;
		
		while ( input.Ready() )
		{
			string line = input.Read();
			
			if ( line.empty()             )  continue;
			if ( line[ 0 ] == ';'         )  continue;
			if ( !eos( line.find( ':' ) ) )  continue;
			
			files.push_back( line.substr( line.find_first_not_of( "\t" ),
			                              string::npos ) );
		}
		
		return files;
	}
	
}


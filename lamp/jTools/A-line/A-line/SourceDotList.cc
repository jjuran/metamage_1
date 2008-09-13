/*	================
 *	SourceDotList.cc
 *	================
 */

#include "A-line/SourceDotList.hh"

// POSeven
#include "POSeven/Open.hh"

// Io
#include "Io/TextInput.hh"

// BitsAndBytes
#include "StringPredicates.hh"


namespace tool
{
	
	namespace NN = Nucleus;
	namespace p7 = poseven;
	
	using BitsAndBytes::eos;
	
	
	std::vector< std::string > ReadSourceDotList( const std::string& pathname )
	{
		Io::TextInputAdapter< NN::Owned< p7::fd_t > > input( io::open_for_reading( pathname ) );
		
		std::vector< std::string > files;
		
		while ( input.Ready() )
		{
			std::string line = input.Read();
			
			if ( line.empty()             )  continue;
			if ( line[ 0 ] == ';'         )  continue;
			if ( line[ 0 ] == '#'         )  continue;
			if ( !eos( line.find( ':' ) ) )  continue;
			
			files.push_back( line.substr( line.find_first_not_of( "\t" ),
			                              line.npos ) );
		}
		
		return files;
	}
	
}


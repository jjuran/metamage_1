/*	================
 *	SourceDotList.cc
 *	================
 */

#include "A-line/SourceDotList.hh"

// POSeven
#include "POSeven/FileDescriptor.hh"

// Nitrogen Extras / Utilities
#include "Utilities/Files.h"

// Io
#include "Io/TextInput.hh"

// BitsAndBytes
#include "StringPredicates.hh"


namespace ALine
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace P7 = POSeven;
	
	using BitsAndBytes::eos;
	
	
	std::vector< std::string > ReadSourceDotList( const FSSpec& sourceList )
	{
		std::string pathname = N::FSpGetPOSIXPathname( sourceList );
		
		Io::TextInputAdapter< NN::Owned< P7::FileDescriptor > > input( io::open_for_reading( pathname ) );
		
		std::vector< std::string > files;
		
		while ( input.Ready() )
		{
			std::string line = input.Read();
			
			if ( line.empty()             )  continue;
			if ( line[ 0 ] == ';'         )  continue;
			if ( !eos( line.find( ':' ) ) )  continue;
			
			files.push_back( line.substr( line.find_first_not_of( "\t" ),
			                              line.npos ) );
		}
		
		return files;
	}
	
}


/*	===============
 *	GetMainEntry.hh
 *	===============
 */

#ifndef GENIE_GETMAINENTRY_HH
#define GENIE_GETMAINENTRY_HH

// Genie
#include "Genie/BinaryImage.hh"
#include "Genie/Exec/MainEntry.hh"


namespace Genie
{
	
	typedef int (*Trivial_Entry )(                       );
	typedef int (*Standard_Entry)( int argc, char** argv );
	
	
	MainEntry GetMainEntryFromAddress( Trivial_Entry  address );
	MainEntry GetMainEntryFromAddress( Standard_Entry address );
	
	MainEntry GetMainEntryFromBinaryImage( const BinaryImage& file );
	
}

#endif


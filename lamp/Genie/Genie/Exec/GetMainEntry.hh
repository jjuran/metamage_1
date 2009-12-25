/*	===============
 *	GetMainEntry.hh
 *	===============
 */

#ifndef GENIE_GETMAINENTRY_HH
#define GENIE_GETMAINENTRY_HH

// Iota
#include "iota/argv.hh"

// Genie
#include "Genie/BinaryImage.hh"
#include "Genie/Exec/MainEntry.hh"


namespace Genie
{
	
	typedef int (*Trivial_Entry )(                             );
	typedef int (*Standard_Entry)( int argc, iota::argv_t argv );
	
	
	MainEntry GetMainEntryFromAddress( Trivial_Entry  address );
	MainEntry GetMainEntryFromAddress( Standard_Entry address );
	
	MainEntry GetMainEntryFromBinaryImage( const BinaryImage& file );
	
}

#endif


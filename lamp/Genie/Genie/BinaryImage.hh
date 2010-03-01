/*	==============
 *	BinaryImage.hh
 *	==============
 */

#ifndef GENIE_BINARYIMAGE_HH
#define GENIE_BINARYIMAGE_HH

// Mac OS
//#include <Files.h>

struct FSSpec;

// nucleus
#include "nucleus/shared.hh"

// Nitrogen
#include "Nitrogen/MacMemory.hh"


namespace Genie
{
	
	typedef nucleus::shared< Nitrogen::Handle > BinaryImage;
	
	BinaryImage GetBinaryImage( const FSSpec& file );
	
}

#endif


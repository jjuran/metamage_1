/*	==============
 *	BinaryImage.hh
 *	==============
 */

#ifndef GENIE_BINARYIMAGE_HH
#define GENIE_BINARYIMAGE_HH

// Mac OS
//#include <Files.h>

struct FSSpec;

// Nucleus
#include "Nucleus/Shared.h"

// Nitrogen
#include "Nitrogen/MacMemory.hh"


namespace Genie
{
	
	typedef Nucleus::Shared< Nitrogen::Handle > BinaryImage;
	
	BinaryImage GetBinaryImage( const FSSpec& file );
	
}

#endif


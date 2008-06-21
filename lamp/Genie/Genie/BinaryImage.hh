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
#include "Nitrogen/MacMemory.h"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	typedef NN::Shared< N::Handle > BinaryImage;
	
	BinaryImage GetBinaryImage( const FSSpec& file );
	
}

#endif


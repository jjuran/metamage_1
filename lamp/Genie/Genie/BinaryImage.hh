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
#include "Mac/Memory/Types/Handle.hh"


namespace Genie
{
	
	typedef nucleus::shared< Mac::Handle > BinaryImage;
	
	BinaryImage GetBinaryImage( const FSSpec& file );
	
}

#endif


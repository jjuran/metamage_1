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
#ifndef NUCLEUS_SHARED_HH
#include "nucleus/shared.hh"
#endif

// Nitrogen
#ifndef MAC_MEMORY_TYPES_HANDLE_HH
#include "Mac/Memory/Types/Handle.hh"
#endif


namespace Genie
{
	
	typedef nucleus::shared< Mac::Handle > BinaryImage;
	
	BinaryImage GetBinaryImage( const FSSpec& file );
	
}

#endif


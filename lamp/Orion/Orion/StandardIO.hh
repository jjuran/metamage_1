/*	=============
 *	StandardIO.hh
 *	=============
 */

#ifndef ORION_STANDARDIO_HH
#define ORION_STANDARDIO_HH

// POSeven
#include "POSeven/FileDescriptor.hh"

// Io
#include "Io/Stream.hh"


namespace Io
{
	
	static Stream< POSeven::FileDescriptor > In ( POSeven::kStdIn_FileNo  ),
	                                         Out( POSeven::kStdOut_FileNo ),
	                                         Err( POSeven::kStdErr_FileNo );
	
}

#endif


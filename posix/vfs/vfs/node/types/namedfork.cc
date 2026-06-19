/*
	namedfork.cc
	------------
*/

#include "vfs/node/types/namedfork.hh"


namespace vfs
{
	
	const unsigned info_sizes[] =
	{
		0,
		2 * 4,    // two 4-byte OSTypes
		16,       // FInfo, which is 16 bytes
		80 - 30,  // size of HFileParam starting at ioFlAttrib
		80 - 32,  // size of HFileParam starting at ioFlFndrInfo
	};
	
}

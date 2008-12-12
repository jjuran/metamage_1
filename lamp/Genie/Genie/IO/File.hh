/*	=======
 *	File.hh
 *	=======
 */

#ifndef GENIE_IO_FILE_HH
#define GENIE_IO_FILE_HH

// Genie
#include "Genie/IO/Stream.hh"


namespace Genie
{
	
	class FileHandle : public StreamHandle
	{
		public:
			FileHandle( OpenFlags flags ) : StreamHandle( flags )
			{
			}
			
			virtual ~FileHandle()  {}
	};
	
}

#endif


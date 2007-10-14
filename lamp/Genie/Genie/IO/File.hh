/*	=======
 *	File.hh
 *	=======
 */

#ifndef GENIE_IO_FILE_HH
#define GENIE_IO_FILE_HH

// Genie
#include "Genie/IO/Stream.hh"

// <sys/stat.h>
struct stat;


namespace Genie
{
	
	class FileHandle : public StreamHandle
	{
		private:
			bool isBlocking;
		
		public:
			FileHandle() : isBlocking( true )  {}
			
			virtual ~FileHandle()  {}
			
			bool IsBlocking() const  { return isBlocking; }
			
			void SetBlocking   ()  { isBlocking = true;  }
			void SetNonBlocking()  { isBlocking = false; }
	};
	
}

#endif


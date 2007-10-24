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
		private:
			bool itIsBlocking;
		
		public:
			FileHandle() : itIsBlocking( true )  {}
			
			virtual ~FileHandle()  {}
			
			bool IsBlocking() const  { return itIsBlocking; }
			
			void SetBlocking   ()  { itIsBlocking = true;  }
			void SetNonBlocking()  { itIsBlocking = false; }
	};
	
}

#endif


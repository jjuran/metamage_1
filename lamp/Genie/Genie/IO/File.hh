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
			bool itIsNonblocking;
		
		public:
			FileHandle() : itIsNonblocking( false )  {}
			
			virtual ~FileHandle()  {}
			
			bool IsNonblocking() const  { return itIsNonblocking; }
			
			void SetNonblocking  ()  { itIsNonblocking = true;  }
			void ClearNonblocking()  { itIsNonblocking = false; }
	};
	
}

#endif


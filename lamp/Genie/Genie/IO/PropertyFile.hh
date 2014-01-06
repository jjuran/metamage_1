/*	===============
 *	PropertyFile.hh
 *	===============
 */

#ifndef GENIE_IO_PROPERTYFILE_HH
#define GENIE_IO_PROPERTYFILE_HH

// vfs
#include "vfs/node_fwd.hh"

// Genie
#include "Genie/IO/Stream.hh"


namespace Genie
{
	
	class PropertyWriterFileHandle : public StreamHandle
	{
		private:
			typedef void (*WriteHook)( const vfs::node*  that,
			                           const char*       begin,
			                           const char*       end,
			                           bool              binary );
			
			WriteHook  itsWriteHook;
			bool       itIsBinary;
		
		public:
			PropertyWriterFileHandle( const vfs::node&  file,
			                          int               flags,
			                          WriteHook         writeHook,
			                          bool              binary );
			
			ssize_t SysWrite( const char* buffer, std::size_t byteCount );
	};
	
}

#endif


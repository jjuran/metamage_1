/*	===============
 *	PropertyFile.hh
 *	===============
 */

#ifndef GENIE_IO_PROPERTYFILE_HH
#define GENIE_IO_PROPERTYFILE_HH

// plus
#include "plus/string.hh"

// vfs
#include "vfs/node_fwd.hh"

// Genie
#include "Genie/IO/RegularFile.hh"


namespace Genie
{
	
	class PropertyReaderFileHandle : public RegularFileHandle
	{
		private:
			plus::string itsData;
		
		public:
			PropertyReaderFileHandle( const vfs::node&     file,
			                          int                  flags,
			                          const plus::string&  value );
			
			ssize_t Positioned_Read( char* buffer, size_t n_bytes, off_t offset );
			
			off_t GetEOF()  { return itsData.size(); }
	};
	
	
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


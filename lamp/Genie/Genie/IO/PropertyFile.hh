/*	===============
 *	PropertyFile.hh
 *	===============
 */

#ifndef GENIE_IO_PROPERTYFILE_HH
#define GENIE_IO_PROPERTYFILE_HH

// plus
#include "plus/string.hh"

// Genie
#include "Genie/IO/RegularFile.hh"
#include "Genie/IO/VirtualFile.hh"


namespace Genie
{
	
	class PropertyReaderFileHandle : public VirtualFileHandle< RegularFileHandle >
	{
		private:
			plus::string itsData;
		
		public:
			PropertyReaderFileHandle( const FSTreePtr&     file,
			                          OpenFlags            flags,
			                          const plus::string&  value )
			:
				VirtualFileHandle< RegularFileHandle >( file, flags ),
				itsData( value )
			{
			}
			
			boost::shared_ptr< IOHandle > Clone();
			
			ssize_t Positioned_Read( char* buffer, size_t n_bytes, off_t offset );
			
			off_t GetEOF()  { return itsData.size(); }
	};
	
	
	class PropertyWriterFileHandle : public VirtualFileHandle< StreamHandle >
	{
		private:
			typedef void (*WriteHook)( const FSTree  *that,
			                           const char    *begin,
			                           const char    *end,
			                           bool           binary );
			
			WriteHook  itsWriteHook;
			bool       itIsBinary;
		
		public:
			PropertyWriterFileHandle( const FSTreePtr&  file,
			                          OpenFlags         flags,
			                          WriteHook         writeHook,
			                          bool              binary )
			:
				VirtualFileHandle< StreamHandle >( file, flags ),
				itsWriteHook( writeHook ),
				itIsBinary( binary )
			{
			}
			
			unsigned int SysPoll()  { return kPollRead | kPollWrite; }
			
			ssize_t SysWrite( const char* buffer, std::size_t byteCount );
	};
	
}

#endif


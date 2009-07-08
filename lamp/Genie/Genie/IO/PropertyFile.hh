/*	===============
 *	PropertyFile.hh
 *	===============
 */

#ifndef GENIE_IO_PROPERTYFILE_HH
#define GENIE_IO_PROPERTYFILE_HH

// Genie
#include "Genie/IO/RegularFile.hh"
#include "Genie/IO/VirtualFile.hh"


namespace Genie
{
	
	class PropertyReaderFileHandle : public VirtualFileHandle< RegularFileHandle >
	{
		private:
			std::string itsData;
		
		public:
			PropertyReaderFileHandle( const FSTreePtr&    file,
			                          OpenFlags           flags,
			                          const std::string&  value )
			:
				VirtualFileHandle( file, flags ),
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
				VirtualFileHandle( file, flags ),
				itsWriteHook( writeHook ),
				itIsBinary( binary )
			{
			}
			
			unsigned int SysPoll()  { return kPollRead | kPollWrite; }
			
			ssize_t SysWrite( const char* buffer, std::size_t byteCount );
	};
	
}

#endif


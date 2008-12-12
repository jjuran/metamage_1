/*	==============
 *	RegularFile.hh
 *	==============
 */

#ifndef GENIE_IO_REGULARFILE_HH
#define GENIE_IO_REGULARFILE_HH

 // Genie
 #include "Genie/IO/File.hh"


namespace Genie
{
	
	class RegularFileHandle : public FileHandle
	{
		private:
			off_t itsMark;
		
		public:
			RegularFileHandle( OpenFlags flags );
			
			virtual ~RegularFileHandle();
			
			bool IsRegularFile() const  { return true; }
			
			virtual unsigned int SysPoll() const  { return kPollRead | kPollWrite; }
			
			off_t Seek( off_t offset, int whence );
			
			virtual off_t GetEOF() const;
			
			virtual void SetEOF( off_t length );
			
			off_t GetFileMark() const  { return itsMark; }
			
			ssize_t Advance( ssize_t step )  { itsMark += step;  return step; }
			
			ssize_t Write( const char* buffer, std::size_t byteCount );
	};
	
	template <> struct IOHandle_Downcast_Traits< RegularFileHandle >
	{
		static IOHandle::Test GetTest()  { return &IOHandle::IsRegularFile; }
		
		static int GetError( IOHandle& handle )
		{
			return IOHandle_Downcast_Traits< StreamHandle >::GetError( handle );
		}
	};
	
}

#endif


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
		public:
			virtual ~RegularFileHandle();
			
			bool IsRegularFile() const  { return true; }
			
			virtual unsigned int SysPoll() const  { return kPollRead | kPollWrite; }
			
			virtual off_t Seek( off_t offset, int whence ) = 0;
			
			virtual off_t GetEOF() const;
			
			virtual void SetEOF( off_t length );
	};
	
	template <> struct IOHandle_Downcast_Traits< RegularFileHandle >
	{
		static IOHandle::Test GetTest()  { return &IOHandle::IsRegularFile; }
	};
	
}

#endif


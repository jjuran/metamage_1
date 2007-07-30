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
			static TypeCode Type()  { return kRegularFileType; }
			
			//TypeCode ActualType() const  { return Type(); }
			
			virtual ~RegularFileHandle();
			
			unsigned int SysPoll() const  { return kPollRead | kPollWrite | kPollExcept; }
			
			virtual off_t Seek( off_t offset, int whence ) = 0;
			
			virtual off_t GetEOF() const;
			
			virtual void SetEOF( off_t length ) const;
	};
	
}

#endif


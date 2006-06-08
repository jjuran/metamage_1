/*	==============
 *	RegularFile.hh
 *	==============
 */

#ifndef GENIE_IO_REGULARFILE_HH
#define GENIE_IO_REGULARFILE_HH

// POSIX
#include "sys/types.h"

// Nucleus
#include "Nucleus/Owned.h"

// Nitrogen
#include "Nitrogen/Files.h"

 // Genie
 #include "Genie/IO/File.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	class RegularFileHandle : public FileHandle
	{
		private:
			NN::Owned< N::FSFileRefNum > refNum;
		
		public:
			static TypeCode Type()  { return kRegularFileType; }
			
			RegularFileHandle( NN::Owned< N::FSFileRefNum > refNum );
			
			TypeCode ActualType() const  { return Type(); }
			
			unsigned int SysPoll() const  { return kPollRead | kPollWrite | kPollExcept; }
			
			int SysRead( char* data, std::size_t byteCount );
			
			int SysWrite( const char* data, std::size_t byteCount );
			
			//void IOCtl( unsigned long request, int* argp );
			
			void Stat( struct stat* sb ) const;
			
			off_t Seek( off_t offset, int whence );
	};
	
}

#endif


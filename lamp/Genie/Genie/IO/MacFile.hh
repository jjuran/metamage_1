/*	==========
 *	MacFile.hh
 *	==========
 */

#ifndef GENIE_IO_MACFILE_HH
#define GENIE_IO_MACFILE_HH

// POSIX
#include "sys/types.h"

// Nucleus
#include "Nucleus/Owned.h"

// Nitrogen
#include "Nitrogen/Files.h"

 // Genie
 #include "Genie/IO/RegularFile.hh"


namespace Genie
{
	
	class MacFileHandle : public RegularFileHandle
	{
		private:
			Nucleus::Owned< Nitrogen::FSFileRefNum > refNum;
			
			OpenFlags  itsOpenFlags;
		
		public:
			MacFileHandle( Nucleus::Owned< Nitrogen::FSFileRefNum > refNum, OpenFlags flags );
			
			~MacFileHandle();
			
			int SysRead( char* data, std::size_t byteCount );
			
			int SysWrite( const char* data, std::size_t byteCount );
			
			//void IOCtl( unsigned long request, int* argp );
			
			off_t Seek( off_t offset, int whence );
			
			off_t GetEOF() const  { return Nitrogen::GetEOF( refNum ); }
			
			void SetEOF( off_t length )  { Nitrogen::SetEOF( refNum, length ); }
			
			FSSpec GetFSSpec( bool forCreation = false ) const;
	};
	
	class MacDataForkHandle : public MacFileHandle
	{
		public:
			MacDataForkHandle( Nucleus::Owned< Nitrogen::FSFileRefNum > refNum, OpenFlags flags ) : MacFileHandle( refNum, flags )
			{
			}
			
			FSTreePtr GetFile() const;
	};
	
	class MacRsrcForkHandle : public MacFileHandle
	{
		public:
			MacRsrcForkHandle( Nucleus::Owned< Nitrogen::FSFileRefNum > refNum, OpenFlags flags ) : MacFileHandle( refNum, flags )
			{
			}
			
			FSTreePtr GetFile() const;
	};
	
}

#endif


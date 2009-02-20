/*	==========
 *	MacFile.hh
 *	==========
 */

#ifndef GENIE_IO_MACFILE_HH
#define GENIE_IO_MACFILE_HH

// POSIX
#include "sys/types.h"

// Nucleus
#include "Nucleus/Shared.h"

// Nitrogen
#include "Nitrogen/Files.h"

// Genie
#include "Genie/IO/RegularFile.hh"


namespace Genie
{
	
	class MacFileHandle : public RegularFileHandle
	{
		private:
			Nucleus::Shared< Nitrogen::FSFileRefNum > itsRefNum;
		
		protected:
			const Nucleus::Shared< Nitrogen::FSFileRefNum >& Get() const
			{
				return itsRefNum;
			}
		
		public:
			MacFileHandle( const Nucleus::Shared< Nitrogen::FSFileRefNum >&  refNum,
			               OpenFlags                                         flags );
			
			~MacFileHandle();
			
			ssize_t SysRead( char* data, std::size_t byteCount );
			
			ssize_t SysWrite( const char* data, std::size_t byteCount );
			
			//void IOCtl( unsigned long request, int* argp );
			
			off_t GetEOF() const  { return Nitrogen::GetEOF( itsRefNum ); }
			
			void SetEOF( off_t length )  { Nitrogen::SetEOF( itsRefNum, length ); }
	};
	
	class MacDataForkHandle : public MacFileHandle
	{
		public:
			MacDataForkHandle( const Nucleus::Shared< Nitrogen::FSFileRefNum >&  refNum,
			                   OpenFlags                                         flags ) : MacFileHandle( refNum, flags )
			{
			}
			
			boost::shared_ptr< IOHandle > Clone();
			
			FSTreePtr GetFile() const;
	};
	
	class MacRsrcForkHandle : public MacFileHandle
	{
		public:
			MacRsrcForkHandle( const Nucleus::Shared< Nitrogen::FSFileRefNum >&  refNum,
			                   OpenFlags                                         flags ) : MacFileHandle( refNum, flags )
			{
			}
			
			boost::shared_ptr< IOHandle > Clone();
			
			FSTreePtr GetFile() const;
	};
	
}

#endif


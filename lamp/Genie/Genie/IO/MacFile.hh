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
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	class MacFileHandle : public RegularFileHandle
	{
		private:
			NN::Owned< N::FSFileRefNum > refNum;
		
		public:
			MacFileHandle( NN::Owned< N::FSFileRefNum > refNum );
			
			~MacFileHandle();
			
			int SysRead( char* data, std::size_t byteCount );
			
			int SysWrite( const char* data, std::size_t byteCount );
			
			//void IOCtl( unsigned long request, int* argp );
			
			off_t Seek( off_t offset, int whence );
			
			off_t GetEOF() const  { return N::GetEOF( refNum ); }
			
			void SetEOF( off_t length )  { N::SetEOF( refNum, length ); }
			
			FSSpec GetFSSpec( bool forCreation = false ) const;
	};
	
	class MacDataForkHandle : public MacFileHandle
	{
		public:
			MacDataForkHandle( NN::Owned< N::FSFileRefNum > refNum ) : MacFileHandle( refNum )  {}
			
			FSTreePtr GetFile() const;
	};
	
	class MacRsrcForkHandle : public MacFileHandle
	{
		public:
			MacRsrcForkHandle( NN::Owned< N::FSFileRefNum > refNum ) : MacFileHandle( refNum )  {}
			
			FSTreePtr GetFile() const;
	};
	
}

#endif


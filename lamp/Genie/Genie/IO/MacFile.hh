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
			static TypeCode Type()  { return kMacFileType; }
			
			TypeCode ActualType() const  { return Type(); }
			
			MacFileHandle( NN::Owned< N::FSFileRefNum > refNum );
			
			~MacFileHandle();
			
			unsigned int SysPoll() const  { return kPollRead | kPollWrite | kPollExcept; }
			
			int SysRead( char* data, std::size_t byteCount );
			
			int SysWrite( const char* data, std::size_t byteCount );
			
			//void IOCtl( unsigned long request, int* argp );
			
			off_t Seek( off_t offset, int whence );
			
			off_t GetEOF() const  { return N::GetEOF( refNum ); }
			
			void SetEOF( off_t length )  { N::SetEOF( refNum, length ); }
			
			FSSpec GetFSSpec() const;
	};
	
	class MacDataForkHandle : public MacFileHandle
	{
		public:
			static TypeCode Type()  { return kMacDataForkType; }
			
			TypeCode ActualType() const  { return Type(); }
			
			MacDataForkHandle( NN::Owned< N::FSFileRefNum > refNum ) : MacFileHandle( refNum )  {}
			
			FSTreePtr GetFile() const;
	};
	
	class MacRsrcForkHandle : public MacFileHandle
	{
		public:
			static TypeCode Type()  { return kMacRsrcForkType; }
			
			TypeCode ActualType() const  { return Type(); }
			
			MacRsrcForkHandle( NN::Owned< N::FSFileRefNum > refNum ) : MacFileHandle( refNum )  {}
			
			FSTreePtr GetFile() const;
	};
	
}

#endif


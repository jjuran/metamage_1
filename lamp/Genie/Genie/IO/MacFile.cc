/*	==========
 *	MacFile.cc
 *	==========
 */

#include "Genie/IO/MacFile.hh"

// Nucleus
#include "Nucleus/NAssert.h"

// Nitrogen
#include "Nitrogen/Files.h"

// Genie
#include "Genie/FileSystem/FSTree_RsrcFile.hh"
#include "Genie/FileSystem/StatFile.hh"


namespace Genie
{
	
	static FSSpec FSSpecFromFRefNum( N::FSFileRefNum refNum )
	{
		FSSpec result;
		
		FCBPBRec pb;
		
		pb.ioVRefNum = 0;
		pb.ioFCBIndx = 0;
		pb.ioRefNum  = refNum;
		pb.ioNamePtr = result.name;
		
		N::ThrowOSStatus( ::PBGetFCBInfoSync( &pb ) );
		
		result.vRefNum = pb.ioFCBVRefNum;
		result.parID   = pb.ioFCBParID;
		
		return result;
	}
	
	MacFileHandle::MacFileHandle( NN::Owned< N::FSFileRefNum > refNum )
	: refNum( refNum )
	{
	}
	
	int MacFileHandle::SysRead( char* data, std::size_t byteCount )
	{
		return N::FSRead( refNum, byteCount, data );
	}
	
	int MacFileHandle::SysWrite( const char* data, std::size_t byteCount )
	{
		return N::FSWrite( refNum, byteCount, data );
	}
	
	void MacFileHandle::Stat( struct stat* sb ) const
	{
		FSSpec file = FSSpecFromFRefNum( refNum );
		
		StatFile( file, sb );
	}
	
	off_t MacFileHandle::Seek( off_t offset, int whence )
	{
		N::FSIOPosMode mode;
		
		switch ( whence )
		{
			case SEEK_SET:
				mode = N::fsFromStart;
				break;
			
			case SEEK_CUR:
				mode = N::fsFromMark;
				break;
			
			case SEEK_END:
				mode = N::fsFromLEOF;
				break;
			
			default:
				//P7::ThrowErrno( EINVAL );
				throw N::ParamErr();
		}
		
		N::SetFPos( refNum, mode, offset );
		
		return N::GetFPos( refNum );
	}
	
	FSSpec MacFileHandle::GetFSSpec() const
	{
		return FSSpecFromFRefNum( refNum );
	}
	
	FSTreePtr MacDataForkHandle::GetFile()
	{
		return FSTreeFromFSSpec( GetFSSpec() );
	}
	
	FSTreePtr MacRsrcForkHandle::GetFile()
	{
		return GetRsrcForkFSTree( GetFSSpec() );
	}
	
}


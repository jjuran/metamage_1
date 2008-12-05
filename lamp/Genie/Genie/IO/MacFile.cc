/*	==========
 *	MacFile.cc
 *	==========
 */

#include "Genie/IO/MacFile.hh"

// POSIX
#include <fcntl.h>

// Nucleus
#include "Nucleus/NAssert.h"

// Nitrogen
#include "Nitrogen/Files.h"

// Genie
#include "Genie/FileSystem/FSTree_RsrcFile.hh"
#include "Genie/FileSystem/StatFile.hh"
#include "Genie/Utilities/AsyncIO.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
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
	
	MacFileHandle::MacFileHandle( NN::Owned< N::FSFileRefNum >  refNum,
	                              OpenFlags                     flags )
	: itsRefNum   ( refNum ),
	  itsOpenFlags( flags  ),
	  itsMark     ( 0      )
	{
	}
	
	MacFileHandle::~MacFileHandle()
	{
	}
	
	int MacFileHandle::SysRead( char* data, std::size_t byteCount )
	{
		ssize_t advance = FSRead( itsRefNum,
		                          N::fsFromStart,
		                          itsMark,
		                          byteCount,
		                          data,
		                          ThrowEOF_Never() );
		
		itsMark += advance;
		
		return advance;
	}
	
	int MacFileHandle::SysWrite( const char* data, std::size_t byteCount )
	{
		if ( (itsOpenFlags & O_TRUNC_LAZY)  &&  itsMark == 0 )
		{
			SetEOF( 0 );
		}
		
		const bool appending = itsOpenFlags & O_APPEND;
		
		const N::FSIOPosMode  mode   = appending ? N::fsFromLEOF : N::fsFromStart;
		const SInt32          offset = appending ? 0             : itsMark;
		
		ssize_t advance =  FSWrite( itsRefNum,
		                            mode,
		                            offset,
		                            byteCount,
		                            data );
		
		itsMark += advance;
		
		return advance;
	}
	
	off_t MacFileHandle::Seek( off_t offset, int whence )
	{
		off_t base = 0;
		
		switch ( whence )
		{
			case SEEK_SET:
				base = 0;
				break;
			
			case SEEK_CUR:
				base = itsMark;
				break;
			
			case SEEK_END:
				base = GetEOF();
				break;
			
			default:
				//p7::throw_errno( EINVAL );
				throw N::ParamErr();
		}
		
		return itsMark = base + offset;
	}
	
	FSSpec MacFileHandle::GetFSSpec( bool forCreation ) const
	{
		return FSSpecFromFRefNum( itsRefNum );
	}
	
	
	FSTreePtr MacDataForkHandle::GetFile() const
	{
		return FSTreeFromFSSpec( GetFSSpec() );
	}
	
	FSTreePtr MacRsrcForkHandle::GetFile() const
	{
		return GetRsrcForkFSTree( GetFSSpec() );
	}
	
}


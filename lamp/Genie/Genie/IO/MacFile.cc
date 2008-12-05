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
	  itsOpenFlags( flags  )
	{
	}
	
	MacFileHandle::~MacFileHandle()
	{
	}
	
	int MacFileHandle::SysRead( char* data, std::size_t byteCount )
	{
		ssize_t read = FSRead( itsRefNum,
		                       N::fsFromStart,
		                       GetFileMark(),
		                       byteCount,
		                       data,
		                       ThrowEOF_Never() );
		
		return Advance( read );
	}
	
	int MacFileHandle::SysWrite( const char* data, std::size_t byteCount )
	{
		if ( (itsOpenFlags & O_TRUNC_LAZY)  &&  GetFileMark() == 0 )
		{
			SetEOF( 0 );
		}
		
		const bool appending = itsOpenFlags & O_APPEND;
		
		const N::FSIOPosMode  mode   = appending ? N::fsFromLEOF : N::fsFromStart;
		const SInt32          offset = appending ? 0             : GetFileMark();
		
		ssize_t written = FSWrite( itsRefNum,
		                           mode,
		                           offset,
		                           byteCount,
		                           data );
		
		return Advance( written );
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


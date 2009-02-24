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
#include "Genie/FileSystem/FSSpec.hh"
#include "Genie/FileSystem/FSTree_RsrcFile.hh"
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
	
	MacFileHandle::MacFileHandle( const NN::Shared< N::FSFileRefNum >&  refNum,
	                              OpenFlags                             flags )
	: RegularFileHandle( flags  ),
	  itsRefNum        ( refNum )
	{
	}
	
	MacFileHandle::~MacFileHandle()
	{
	}
	
	ssize_t MacFileHandle::SysRead( char* data, std::size_t byteCount )
	{
		ssize_t read = FSRead( itsRefNum,
		                       N::fsFromStart,
		                       GetFileMark(),
		                       byteCount,
		                       data,
		                       ThrowEOF_Never() );
		
		return Advance( read );
	}
	
	ssize_t MacFileHandle::SysWrite( const char* data, std::size_t byteCount )
	{
		const bool appending = GetFlags() & O_APPEND;
		
		const N::FSIOPosMode  mode   = appending ? N::fsFromLEOF : N::fsFromStart;
		const SInt32          offset = appending ? 0             : GetFileMark();
		
		ssize_t written = FSWrite( itsRefNum,
		                           mode,
		                           offset,
		                           byteCount,
		                           data );
		
		return Advance( written );
	}
	
	
	boost::shared_ptr< IOHandle > MacDataForkHandle::Clone()
	{
		return boost::shared_ptr< IOHandle >( new MacDataForkHandle( Get(), GetFlags() ) );
	}
	
	FSTreePtr MacDataForkHandle::GetFile() const
	{
		return FSTreeFromFSSpec( FSSpecFromFRefNum( Get() ) );
	}
	
	
	boost::shared_ptr< IOHandle > MacRsrcForkHandle::Clone()
	{
		return boost::shared_ptr< IOHandle >( new MacRsrcForkHandle( Get(), GetFlags() ) );
	}
	
	FSTreePtr MacRsrcForkHandle::GetFile() const
	{
		return GetRsrcForkFSTree( FSSpecFromFRefNum( Get() ) );
	}
	
}


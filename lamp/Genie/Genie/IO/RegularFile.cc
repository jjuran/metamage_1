/*	==============
 *	RegularFile.cc
 *	==============
 */

#include "Genie/IO/RegularFile.hh"

// Nucleus
#include "Nucleus/NAssert.h"

// Nitrogen
#include "Nitrogen/Files.h"

// Io
#include "Io/Exceptions.hh"

// Genie
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
	
	RegularFileHandle::RegularFileHandle( NN::Owned< N::FSFileRefNum > refNum )
	: refNum( refNum )
	{
	}
	
	int RegularFileHandle::SysRead( char* data, std::size_t byteCount )
	{
		try
		{
			return N::FSRead( refNum, byteCount, data );
		}
		catch ( const N::EOFErr& err )
		{
			ASSERT( err.Get() == eofErr );
			
			throw Io::EndOfInput();
		}
		
		// Not reached
		return 0;
	}
	
	int RegularFileHandle::SysWrite( const char* data, std::size_t byteCount )
	{
		return N::FSWrite( refNum, byteCount, data );
	}
	
	void RegularFileHandle::Stat( struct stat* sb ) const
	{
		FSSpec file = FSSpecFromFRefNum( refNum );
		
		StatFile( file, sb );
	}
	
	off_t RegularFileHandle::Seek( off_t offset, int whence )
	{
		N::FSIOPosMode mode;
		
		switch ( whence )
		{
			case SEEK_SET:
				mode = fsFromStart;
				break;
			
			case SEEK_CUR:
				mode = fsFromMark;
				break;
			
			case SEEK_END:
				mode = fsFromLEOF;
				break;
			
			default:
				//P7::ThrowErrno( EINVAL );
				throw N::ParamErr();
		}
		
		N::SetFPos( refNum, mode, offset );
		
		return N::GetFPos( refNum );
	}
	
}


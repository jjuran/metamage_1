/*	==========
 *	MacFile.cc
 *	==========
 */

#include "Genie/IO/MacFile.hh"

// Genie
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/FSTree_RsrcFile.hh"
#include "Genie/IO/RegularFile.hh"
#include "Genie/Utilities/AsyncIO.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
	class MacFileHandle : public RegularFileHandle
	{
		private:
			typedef FSTreePtr (*FileGetter)( const FSSpec&, bool );
			
			Nucleus::Shared< Nitrogen::FSFileRefNum >  itsRefNum;
			FileGetter                                 itsFileGetter;
		
		public:
			MacFileHandle( const Nucleus::Shared< Nitrogen::FSFileRefNum >&  refNum,
			               OpenFlags                                         flags,
			               FileGetter                                        getFile );
			
			~MacFileHandle();
			
			FSTreePtr GetFile();
			
			boost::shared_ptr< IOHandle > Clone();
			
			ssize_t Positioned_Read( char* buffer, size_t n_bytes, off_t offset );
			
			ssize_t Positioned_Write( const char* buffer, size_t n_bytes, off_t offset );
			
			ssize_t Append( const char* buffer, size_t n_bytes );
			
			//void IOCtl( unsigned long request, int* argp );
			
			void Synchronize( bool metadata );
			
			off_t GetEOF()  { return Nitrogen::GetEOF( itsRefNum ); }
			
			void SetEOF( off_t length )  { Nitrogen::SetEOF( itsRefNum, length ); }
	};
	
	
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
	                              OpenFlags                             flags,
	                              FileGetter                            getFile )
	: RegularFileHandle( flags  ),
	  itsRefNum        ( refNum ),
	  itsFileGetter    ( getFile )
	{
	}
	
	MacFileHandle::~MacFileHandle()
	{
	}
	
	FSTreePtr MacFileHandle::GetFile()
	{
		const bool async = GetFlags() & O_MAC_ASYNC;
		
		return itsFileGetter( FSSpecFromFRefNum( itsRefNum ), async );
	}
	
	boost::shared_ptr< IOHandle > MacFileHandle::Clone()
	{
		return seize_ptr( new MacFileHandle( itsRefNum,
		                                     GetFlags(),
		                                     itsFileGetter ) );
	}
	
	ssize_t MacFileHandle::Positioned_Read( char* data, size_t byteCount, off_t offset )
	{
		const bool async = GetFlags() & O_MAC_ASYNC;
		
		ssize_t read = FSRead( MacIO::kThrowEOF_Never,
		                       itsRefNum,
		                       N::fsFromStart,
		                       offset,
		                       byteCount,
		                       data,
		                       async );
		
		return read;
	}
	
	ssize_t MacFileHandle::Positioned_Write( const char* data, size_t byteCount, off_t offset )
	{
		const bool async = GetFlags() & O_MAC_ASYNC;
		
		const N::FSIOPosMode mode = N::fsFromStart;
		
		ssize_t written = FSWrite( itsRefNum,
		                           mode,
		                           offset,
		                           byteCount,
		                           data,
		                           async );
		
		return written;
	}
	
	ssize_t MacFileHandle::Append( const char* data, size_t byteCount )
	{
		const bool async = GetFlags() & O_MAC_ASYNC;
		
		const N::FSIOPosMode  mode   = N::fsFromLEOF;
		const SInt32          offset = 0;
		
		ssize_t written = FSWrite( itsRefNum,
		                           mode,
		                           offset,
		                           byteCount,
		                           data,
		                           async );
		
		return written;
	}
	
	void MacFileHandle::Synchronize( bool metadata )
	{
		metadata = true;  // until we implement data-only flush
		
		if ( metadata )
		{
			FSSpec file = FSSpecFromFRefNum( itsRefNum );
			
			// Just flush the whole volume, since we can't be more specific.
			N::ThrowOSStatus( ::FlushVol( NULL, file.vRefNum ) );
		}
		else
		{
			// Call PBFlushFile(), or high-level wrapper
		}
	}
	
	
	boost::shared_ptr< IOHandle >
	//
	New_DataForkHandle( const NN::Shared< N::FSFileRefNum >&  refNum,
	                    OpenFlags                             flags )
	{
		return seize_ptr( new MacFileHandle( refNum,
		                                     flags,
		                                     &FSTreeFromFSSpec ) );
	}
	
	boost::shared_ptr< IOHandle >
	//
	New_RsrcForkHandle( const NN::Shared< N::FSFileRefNum >&  refNum,
	                    OpenFlags                             flags )
	{
		return seize_ptr( new MacFileHandle( refNum,
		                                     flags,
		                                     &GetRsrcForkFSTree ) );
	}
	
}


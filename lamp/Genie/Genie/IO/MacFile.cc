/*	==========
 *	MacFile.cc
 *	==========
 */

#include "Genie/IO/MacFile.hh"

// Nitrogen
#include "Nitrogen/Files.hh"

// Genie
#include "Genie/FileSignature.hh"
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/FSTree_RsrcFile.hh"
#include "Genie/IO/RegularFile.hh"
#include "Genie/Utilities/AsyncIO.hh"
#include "Genie/Utilities/is_binary_data.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	
	class MacFileHandle : public RegularFileHandle
	{
		private:
			typedef FSTreePtr (*FileGetter)( const FSSpec&, bool );
			
			nucleus::shared< Nitrogen::FSFileRefNum >  itsRefNum;
			FileGetter                                 itsFileGetter;
		
		public:
			MacFileHandle( const nucleus::shared< Nitrogen::FSFileRefNum >&  refNum,
			               OpenFlags                                         flags,
			               FileGetter                                        getFile );
			
			~MacFileHandle();
			
			FSTreePtr GetFile();
			
			IOPtr Clone();
			
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
		
		Mac::ThrowOSStatus( ::PBGetFCBInfoSync( &pb ) );
		
		result.vRefNum = pb.ioFCBVRefNum;
		result.parID   = pb.ioFCBParID;
		
		return result;
	}
	
	static bool ToggleBinaryFileSignature( FInfo& fInfo, bool to_binary )
	{
		const ::OSType creators[] = { TextFileCreator(), '\?\?\?\?' };
		const ::OSType types   [] = { 'TEXT',            '\?\?\?\?' };
		
		if ( fInfo.fdType == types[ !to_binary ] )
		{
			fInfo.fdCreator = creators[ to_binary ];
			fInfo.fdType    = types   [ to_binary ];
			
			return true;
		}
		
		return false;
	}
	
	static void ToggleBinaryFileSignature( const FSSpec& file, bool to_binary )
	{
		FInfo fInfo = N::FSpGetFInfo( file );
		
		if ( ToggleBinaryFileSignature( fInfo, to_binary ) )
		{
			N::FSpSetFInfo( file, fInfo );
		}
	}
	
	static void CheckFileSignature( const FSSpec& file, const char* data, size_t n_bytes )
	{
		try
		{
			ToggleBinaryFileSignature( file, is_binary_data( data, n_bytes ) );
		}
		catch ( ... )
		{
		}
	}
	
	
	MacFileHandle::MacFileHandle( const n::shared< N::FSFileRefNum >&  refNum,
	                              OpenFlags                            flags,
	                              FileGetter                           getFile )
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
	
	IOPtr MacFileHandle::Clone()
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
		
		if ( offset == 0 )
		{
			CheckFileSignature( FSSpecFromFRefNum( itsRefNum ), data, byteCount );
		}
		
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
			Mac::ThrowOSStatus( ::FlushVol( NULL, file.vRefNum ) );
		}
		else
		{
			// Call PBFlushFile(), or high-level wrapper
		}
	}
	
	
	IOPtr
	//
	New_DataForkHandle( const n::shared< N::FSFileRefNum >&  refNum,
	                    OpenFlags                            flags )
	{
		return seize_ptr( new MacFileHandle( refNum,
		                                     flags,
		                                     &FSTreeFromFSSpec ) );
	}
	
	IOPtr
	//
	New_RsrcForkHandle( const n::shared< N::FSFileRefNum >&  refNum,
	                    OpenFlags                            flags )
	{
		return seize_ptr( new MacFileHandle( refNum,
		                                     flags,
		                                     &GetRsrcForkFSTree ) );
	}
	
}


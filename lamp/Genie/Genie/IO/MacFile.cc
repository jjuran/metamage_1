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
	
	static void ToggleBinaryFileSignature( const FSSpec& file, bool to_binary )
	{
		const ::OSType creators[] = { TextFileCreator(), '\?\?\?\?' };
		const ::OSType types   [] = { 'TEXT',            '\?\?\?\?' };
		
		FInfo fInfo = N::FSpGetFInfo( file );
		
		if ( fInfo.fdType == types[ !to_binary ] )
		{
			fInfo.fdCreator = creators[ to_binary ];
			fInfo.fdType    = types   [ to_binary ];
			
			N::FSpSetFInfo( file, fInfo );
		}
	}
	
	static void CheckFileSignature( const FSSpec& file, const char* data, size_t n_bytes )
	{
		// bitmap of which control characters make a file 'binary' (i.e. non-text)
		
		const unsigned mask = 1 << '\x00'
		                    | 1 << '\x01'
		                    | 1 << '\x02'
		                    | 1 << '\x03'
		                    | 1 << '\x04'
		                    | 1 << '\x05'
		                    | 1 << '\x06'
		                    | 1 << '\x07'
		                    | 1 << '\x08'
		                    | 0 << '\t'  // Tab
		                    | 0 << '\n'  // Line Feed
		                    | 1 << '\x0b'
		                    | 1 << '\x0c'
		                    | 0 << '\r'  // Return
		                    | 1 << '\x0e'
		                    | 1 << '\x0f'
		                    | 1 << '\x10'
		                    | 1 << '\x11'
		                    | 1 << '\x12'
		                    | 1 << '\x13'
		                    | 1 << '\x14'
		                    | 1 << '\x15'
		                    | 1 << '\x16'
		                    | 1 << '\x17'
		                    | 1 << '\x18'
		                    | 1 << '\x19'
		                    | 1 << '\x1a'
		                    | 1 << '\x1b'
		                    | 1 << '\x1c'
		                    | 1 << '\x1d'
		                    | 1 << '\x1e'
		                    | 1 << '\x1f';
		
		// Enough of the file header to tell if it's 'binary'.
		// PEF is text through the first 12, so make it 16.
		const size_t enough_bytes = 16;
		
		const char* end = data + std::min( n_bytes, enough_bytes );
		
		bool is_binary = false;
		
		for ( const char* p = data;  p < end;  ++p )
		{
			const char c = *p;
			
			if ( c < 32  &&  1 << c & mask )
			{
				// Control character found.
				// Set type/creator for a 'binary' file.
				
				is_binary = true;
				
				break;
			}
		}
		
		try
		{
			ToggleBinaryFileSignature( file, is_binary );
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
			N::ThrowOSStatus( ::FlushVol( NULL, file.vRefNum ) );
		}
		else
		{
			// Call PBFlushFile(), or high-level wrapper
		}
	}
	
	
	boost::shared_ptr< IOHandle >
	//
	New_DataForkHandle( const n::shared< N::FSFileRefNum >&  refNum,
	                    OpenFlags                            flags )
	{
		return seize_ptr( new MacFileHandle( refNum,
		                                     flags,
		                                     &FSTreeFromFSSpec ) );
	}
	
	boost::shared_ptr< IOHandle >
	//
	New_RsrcForkHandle( const n::shared< N::FSFileRefNum >&  refNum,
	                    OpenFlags                            flags )
	{
		return seize_ptr( new MacFileHandle( refNum,
		                                     flags,
		                                     &GetRsrcForkFSTree ) );
	}
	
}


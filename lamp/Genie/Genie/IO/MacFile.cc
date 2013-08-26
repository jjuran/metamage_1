/*	==========
 *	MacFile.cc
 *	==========
 */

#include "Genie/IO/MacFile.hh"

// vfs
#include "vfs/filehandle/methods/bstore_method_set.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"

// Nitrogen
#include "Nitrogen/Files.hh"

// MacIO
#include "MacIO/FSRead_Sync.hh"
#include "MacIO/FSWrite_Sync.hh"

// Genie
#include "Genie/FileSignature.hh"
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/FSTree.hh"
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
			typedef FSTreePtr (*FileGetter)( const FSSpec& );
			
			nucleus::shared< Nitrogen::FSFileRefNum >  itsRefNum;
			FileGetter                                 itsFileGetter;
		
		public:
			MacFileHandle( const nucleus::shared< Nitrogen::FSFileRefNum >&  refNum,
			               int                                               flags,
			               FileGetter                                        getFile );
			
			~MacFileHandle();
			
			FSTreePtr GetFile();
			
			ssize_t Positioned_Read( char* buffer, size_t n_bytes, off_t offset );
			
			ssize_t Positioned_Write( const char* buffer, size_t n_bytes, off_t offset );
			
			ssize_t Append( const char* buffer, size_t n_bytes );
			
			void Synchronize( bool metadata );
			
			off_t GetEOF()  { return Nitrogen::GetEOF( itsRefNum ); }
			
			void SetEOF( off_t length )  { Nitrogen::SetEOF( itsRefNum, length ); }
	};
	
	
	static ssize_t hfs_pread( vfs::filehandle* file, char* buffer, size_t n, off_t offset )
	{
		return static_cast< MacFileHandle& >( *file ).Positioned_Read( buffer, n, offset );
	}
	
	static off_t hfs_geteof( vfs::filehandle* file )
	{
		return static_cast< MacFileHandle& >( *file ).GetEOF();
	}
	
	static ssize_t hfs_pwrite( vfs::filehandle* file, const char* buffer, size_t n, off_t offset )
	{
		return static_cast< MacFileHandle& >( *file ).Positioned_Write( buffer, n, offset );
	}
	
	static void hfs_seteof( vfs::filehandle* file, off_t length )
	{
		static_cast< MacFileHandle& >( *file ).SetEOF( length );
	}
	
	static ssize_t hfs_append( vfs::filehandle* file, const char* buffer, size_t n )
	{
		return static_cast< MacFileHandle& >( *file ).Append( buffer, n );
	}
	
	static const vfs::bstore_method_set hfs_bstore_methods =
	{
		&hfs_pread,
		&hfs_geteof,
		&hfs_pwrite,
		&hfs_seteof,
		&hfs_append,
	};
	
	static const vfs::filehandle_method_set hfs_methods =
	{
		&hfs_bstore_methods,
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
	                              int                                  flags,
	                              FileGetter                           getFile )
	: RegularFileHandle( flags, &hfs_methods ),
	  itsRefNum        ( refNum ),
	  itsFileGetter    ( getFile )
	{
	}
	
	MacFileHandle::~MacFileHandle()
	{
	}
	
	FSTreePtr MacFileHandle::GetFile()
	{
		return itsFileGetter( FSSpecFromFRefNum( itsRefNum ) );
	}
	
	ssize_t MacFileHandle::Positioned_Read( char* data, size_t byteCount, off_t offset )
	{
		ssize_t read = MacIO::FSRead( MacIO::kThrowEOF_Never,
		                              itsRefNum,
		                              N::fsFromStart,
		                              offset,
		                              byteCount,
		                              data );
		
		return read;
	}
	
	ssize_t MacFileHandle::Positioned_Write( const char* data, size_t byteCount, off_t offset )
	{
		const N::FSIOPosMode mode = N::fsFromStart;
		
		ssize_t written = MacIO::FSWrite( itsRefNum,
		                                  mode,
		                                  offset,
		                                  byteCount,
		                                  data );
		
		if ( offset == 0 )
		{
			CheckFileSignature( FSSpecFromFRefNum( itsRefNum ), data, byteCount );
		}
		
		return written;
	}
	
	ssize_t MacFileHandle::Append( const char* data, size_t byteCount )
	{
		const N::FSIOPosMode  mode   = N::fsFromLEOF;
		const SInt32          offset = 0;
		
		ssize_t written = MacIO::FSWrite( itsRefNum,
		                                  mode,
		                                  offset,
		                                  byteCount,
		                                  data );
		
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
	                    int                                  flags )
	{
		return new MacFileHandle( refNum,
		                          flags,
		                          &FSTreeFromFSSpec );
	}
	
	IOPtr
	//
	New_RsrcForkHandle( const n::shared< N::FSFileRefNum >&  refNum,
	                    int                                  flags )
	{
		return new MacFileHandle( refNum,
		                          flags,
		                          &GetRsrcForkFSTree );
	}
	
}


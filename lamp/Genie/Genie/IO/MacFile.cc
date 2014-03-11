/*	==========
 *	MacFile.cc
 *	==========
 */

#include "Genie/IO/MacFile.hh"

// POSIX
#include <fcntl.h>

// Standard C
#include <string.h>

// gear
#include "gear/is_binary_data.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/filehandle/methods/bstore_method_set.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"

// Nitrogen
#include "Nitrogen/Files.hh"

// MacIO
#include "MacIO/FSRead_Sync.hh"
#include "MacIO/FSWrite_Sync.hh"

// Genie
#include "Genie/FileSignature.hh"
#include "Genie/FS/FSTree.hh"
#include "Genie/Utilities/AsyncIO.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	
	class MacFileHandle : public vfs::filehandle
	{
		private:
			typedef FSTreePtr (*FileGetter)( const FSSpec& );
			
			n::owned< Nitrogen::FSFileRefNum >  itsRefNum;
			FileGetter                          itsFileGetter;
		
		public:
			MacFileHandle( n::owned< Nitrogen::FSFileRefNum >&  refNum,
			               int                                  flags,
			               FileGetter                           getFile );
			
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
		MacFileHandle& h = static_cast< MacFileHandle& >( *file );
		
		const off_t eof = h.GetEOF();
		
		h.SetEOF( length );
		
		if ( length > eof )
		{
			const off_t block_size = 4096;
			
			char buffer[ block_size ];
			
			memset( buffer, '\0', block_size );
			
			off_t block_offset = (eof + block_size - 1) & ~(block_size - 1);
			
			if ( length <= block_offset )
			{
				h.Positioned_Write( buffer, length - eof, eof );
				
				return;
			}
			
			h.Positioned_Write( buffer, block_offset - eof, eof );
			
			off_t end_block_offset = length & ~(block_size - 1);
			
			while ( block_offset < end_block_offset )
			{
				h.Positioned_Write( buffer, block_size, block_offset );
				
				block_offset += block_size;
			}
			
			if ( block_offset < length )
			{
				h.Positioned_Write( buffer, length - block_offset, block_offset );
			}
		}
	}
	
	static ssize_t hfs_append( vfs::filehandle* file, const char* buffer, size_t n )
	{
		return static_cast< MacFileHandle& >( *file ).Append( buffer, n );
	}
	
	static void hfs_fsync( vfs::filehandle* file )
	{
		return static_cast< MacFileHandle& >( *file ).Synchronize( true );
	}
	
	static const vfs::bstore_method_set hfs_bstore_methods =
	{
		&hfs_pread,
		&hfs_geteof,
		&hfs_pwrite,
		&hfs_seteof,
		&hfs_append,
		&hfs_fsync,
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
			ToggleBinaryFileSignature( file, gear::is_binary_data( data, n_bytes ) );
		}
		catch ( ... )
		{
		}
	}
	
	
	MacFileHandle::MacFileHandle( n::owned< N::FSFileRefNum >&  refNum,
	                              int                           flags,
	                              FileGetter                    getFile )
	: vfs::filehandle( flags, &hfs_methods ),
	  itsRefNum      ( refNum ),
	  itsFileGetter  ( getFile )
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
	
	
	vfs::filehandle_ptr
	//
	OpenMacFileHandle( const FSSpec&  fileSpec,
	                   int            flags,
	                   ForkOpener     openFork,
	                   FileGetter     getFile )
	{
		Mac::FSIOPermissions rdPerm = Mac::FSIOPermissions( flags + 1 - O_RDONLY  &  FREAD  );
		Mac::FSIOPermissions wrPerm = Mac::FSIOPermissions( flags + 1 - O_RDONLY  &  FWRITE );
		
		n::owned< Mac::FSFileRefNum > fileHandle = openFork( fileSpec, rdPerm | wrPerm );
		
		return new MacFileHandle( fileHandle, flags, getFile );
	}
	
}


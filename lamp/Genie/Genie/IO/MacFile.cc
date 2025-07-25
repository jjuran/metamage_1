/*	==========
 *	MacFile.cc
 *	==========
 */

#include "Genie/IO/MacFile.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// POSIX
#include <fcntl.h>

// Standard C
#include <string.h>

// gear
#include "gear/is_binary_data.hh"

// mac-file-utils
#include "mac_file/refnum_file.hh"
#include "mac_file/rw.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/filehandle/methods/bstore_method_set.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/general_method_set.hh"

// Nitrogen
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"

// Genie
#include "Genie/FileSignature.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	using mac::file::refnum_file;
	
	
	typedef vfs::node_ptr (*FileGetter)( const FSSpec& );
	
	struct Mac_file_extra
	{
		FileGetter  getfile;
		short       refnum;
	};
	
	static
	void close_Mac_file( vfs::filehandle* that )
	{
		Mac_file_extra& extra = *(Mac_file_extra*) that->extra();
		
		::FSClose( extra.refnum );
	}
	
	
	static
	ssize_t hfs_pread( vfs::filehandle*  that,
	                   char*             buffer,
	                   size_t            n,
	                   off_t             offset );
	
	static off_t hfs_geteof( vfs::filehandle* file )
	{
		Mac_file_extra& extra = *(Mac_file_extra*) file->extra();
		
		Size size;
		Mac::ThrowOSStatus( ::GetEOF( extra.refnum, &size ) );
		
		return size;
	}
	
	static
	ssize_t hfs_pwrite( vfs::filehandle*  that,
	                    const char*       buffer,
	                    size_t            n,
	                    off_t             offset );
	
	static void hfs_seteof( vfs::filehandle* file, off_t length )
	{
		Mac_file_extra& extra = *(Mac_file_extra*) file->extra();
		
		const off_t eof = hfs_geteof( file );
		
		Mac::ThrowOSStatus( ::SetEOF( extra.refnum, length ) );
		
		if ( length > eof )
		{
			const off_t block_size = 4096;
			
			char buffer[ block_size ];
			
			memset( buffer, '\0', block_size );
			
			off_t block_offset = (eof + block_size - 1) & ~(block_size - 1);
			
			if ( length <= block_offset )
			{
				hfs_pwrite( file, buffer, length - eof, eof );
				
				return;
			}
			
			hfs_pwrite( file, buffer, block_offset - eof, eof );
			
			off_t end_block_offset = length & ~(block_size - 1);
			
			while ( block_offset < end_block_offset )
			{
				hfs_pwrite( file, buffer, block_size, block_offset );
				
				block_offset += block_size;
			}
			
			if ( block_offset < length )
			{
				hfs_pwrite( file, buffer, length - block_offset, block_offset );
			}
		}
	}
	
	static
	ssize_t hfs_append( vfs::filehandle* that, const char* buffer, size_t n );
	
	static
	void hfs_fsync( vfs::filehandle* that );
	
	static
	vfs::node_ptr hfs_getfile( vfs::filehandle* that );
	
	static const vfs::bstore_method_set hfs_bstore_methods =
	{
		&hfs_pread,
		&hfs_geteof,
		&hfs_pwrite,
		&hfs_seteof,
		&hfs_append,
		&hfs_fsync,
	};
	
	static const vfs::general_method_set hfs_general_methods =
	{
		NULL,
		NULL,
		&hfs_getfile,
	};
	
	static const vfs::filehandle_method_set hfs_methods =
	{
		&hfs_bstore_methods,
		NULL,
		NULL,
		&hfs_general_methods,
	};
	
	
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
	
	static void CheckFileSignature( const FSSpec& file, const char* data, size_t n_bytes )
	{
		OSErr err;
		FInfo fInfo;
		
		err = HGetFInfo( file.vRefNum, file.parID, file.name, &fInfo );
		
		if ( err == noErr )
		{
			const bool to_binary = gear::is_binary_data( data, n_bytes );
			
			if ( ToggleBinaryFileSignature( fInfo, to_binary ) )
			{
				err = HSetFInfo( file.vRefNum, file.parID, file.name, &fInfo );
			}
		}
		
		// ignore errors
	}
	
	
	static
	vfs::filehandle* new_Mac_filehandle( short       refNum,
	                                     int         flags,
	                                     FileGetter  getFile )
	{
		vfs::filehandle* result = NULL;
		
		try
		{
			result = new vfs::filehandle( NULL,
			                              flags,
			                              &hfs_methods,
			                              sizeof (Mac_file_extra),
			                              &close_Mac_file );
		}
		catch ( ... )
		{
			FSClose( refNum );
			
			throw;
		}
		
		Mac_file_extra& extra = *(Mac_file_extra*) result->extra();
		
		extra.getfile = getFile;
		extra.refnum  = refNum;
		
		return result;
	}
	
	static
	vfs::node_ptr hfs_getfile( vfs::filehandle* that )
	{
		Mac_file_extra& extra = *(Mac_file_extra*) that->extra();
		
		return extra.getfile( refnum_file( extra.refnum ) );
	}
	
	static
	ssize_t hfs_pread( vfs::filehandle*  that,
	                   char*             buffer,
	                   size_t            n,
	                   off_t             offset )
	{
		Mac_file_extra& extra = *(Mac_file_extra*) that->extra();
		
		const bool readable = (that->get_flags() + 1 - O_RDONLY) & 1;
		
		if ( !readable )
		{
			p7::throw_errno( EBADF );
		}
		
		long read = mac::file::read( extra.refnum, buffer, n, offset );
		
		if ( read < 0 )
		{
			Mac::ThrowOSStatus( read );
		}
		
		return read;
	}
	
	static
	ssize_t hfs_pwrite( vfs::filehandle*  that,
	                    const char*       buffer,
	                    size_t            n,
	                    off_t             offset )
	{
		Mac_file_extra& extra = *(Mac_file_extra*) that->extra();
		
		if ( offset > hfs_geteof( that ) )
		{
			hfs_seteof( that, offset );
		}
		
		OSErr err = mac::file::write( extra.refnum, buffer, n, offset );
		
		Mac::ThrowOSStatus( err );
		
		if ( offset == 0 )
		{
			CheckFileSignature( refnum_file( extra.refnum ), buffer, n );
		}
		
		return n;
	}
	
	static
	ssize_t hfs_append( vfs::filehandle* that, const char* buffer, size_t n )
	{
		Mac_file_extra& extra = *(Mac_file_extra*) that->extra();
		
		OSErr err = mac::file::append( extra.refnum, buffer, n );
		
		Mac::ThrowOSStatus( err );
		
		return n;
	}
	
	static
	void hfs_fsync( vfs::filehandle* that )
	{
		Mac_file_extra& extra = *(Mac_file_extra*) that->extra();
		
		const bool metadata = true;  // until we implement data-only flush
		
		if ( metadata )
		{
			FSSpec file = refnum_file( extra.refnum );
			
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
		const bool writable = (flags + 1 - O_RDONLY) & 2;
		
		const SInt8 perm = writable ? fsRdWrShPerm : fsRdPerm;
		
		short refnum = openFork( fileSpec, perm );
		
		if ( refnum < 0 )
		{
			Mac::ThrowOSStatus( refnum );
		}
		
		return new_Mac_filehandle( refnum, flags, getFile );
	}
	
}

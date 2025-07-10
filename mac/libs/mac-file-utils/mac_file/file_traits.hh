/*
	file_traits.hh
	--------------
*/

#ifndef MACFILE_FILETRAITS_HH
#define MACFILE_FILETRAITS_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// mac-file-utils
#include "mac_file/FSIORefNum.hh"


namespace mac  {
namespace file {

static inline
bool is_read_error( OSErr err )
{
	return err != noErr  &&  err != eofErr;
}

template < class File >
struct file_traits;

#if ! __LP64__

template <>
struct file_traits< FSSpec >
{
	typedef FSSpec File;
	typedef FInfo  FileInfo;
	typedef SInt32 file_size_t;
	
	typedef ConstStr255Param Name;
	
	static ConstStr255Param get_name( const File& file )
	{
		return file.name;
	}
	
	static OSErr close( FSIORefNum refnum )
	{
		return FSClose( refnum );
	}
	
	static Size read( FSIORefNum refnum, void* buffer, SInt32 n )
	{
		OSErr err = FSRead( refnum, &n, buffer );
		
		return is_read_error( err ) ? err : n;
	}
	
	static Size write( FSIORefNum refnum, const void* buffer, SInt32 n )
	{
		OSErr err = FSWrite( refnum, &n, buffer );
		
		return err ? err : n;
	}
	
	static SInt32 geteof( FSIORefNum refnum )
	{
		SInt32 result;
		
		OSErr err = GetEOF( refnum, &result );
		
		return err ? err : result;
	}
	
	static OSErr seteof( FSIORefNum refnum )
	{
		OSErr  err;
		SInt32 pos;
		
		(err = GetFPos( refnum, &pos ))  ||
		(err = SetEOF ( refnum,  pos ));
		
		return err;
	}
};

#endif

template <>
struct file_traits< FSRef >
{
	typedef FSRef         File;
	typedef FSCatalogInfo FileInfo;
	typedef SInt64        file_size_t;
	
	typedef HFSUniStr255 Name;
	
	static HFSUniStr255 get_name( const File& file )
	{
		HFSUniStr255 result = {};
		
		OSErr err = FSGetCatalogInfo( &file, 0, NULL, &result, NULL, NULL );
		
		return result;
	}
	
	static OSErr close( FSIORefNum refnum )
	{
		return FSCloseFork( refnum );
	}
	
	static long read( FSIORefNum refnum, void* buffer, ByteCount n )
	{
		OSErr err = FSReadFork( refnum, fsAtMark, 0, n, buffer, &n );
		
		return is_read_error( err ) ? err : n;
	}
	
	static long write( FSIORefNum refnum, const void* buffer, ByteCount n )
	{
		OSErr err = FSWriteFork( refnum, fsAtMark, 0, n, buffer, &n );
		
		return err ? err : n;
	}
	
	static SInt64 geteof( FSIORefNum refnum )
	{
		SInt64 result;
		
		OSErr err = FSGetForkSize( refnum, &result );
		
		return err ? err : result;
	}
	
	static OSErr seteof( FSIORefNum refnum )
	{
		return FSSetForkSize( refnum, fsAtMark, 0 );
	}
};

template < class File >
inline
typename file_traits< File >::Name get_name( const File& file )
{
	return file_traits< File >::get_name( file );
}

}
}

#endif

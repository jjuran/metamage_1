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

template < class File >
struct file_traits;

template <>
struct file_traits< FSSpec >
{
	typedef FSSpec File;
	typedef SInt32 file_size_t;
	
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
		
		return err ? err : n;
	}
	
	static SInt32 geteof( FSIORefNum refnum )
	{
		SInt32 result;
		
		OSErr err = GetEOF( refnum, &result );
		
		return err ? err : result;
	}
};

template <>
struct file_traits< FSRef >
{
	typedef FSRef  File;
	typedef SInt64 file_size_t;
	
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
		
		return err ? err : n;
	}
	
	static SInt64 geteof( FSIORefNum refnum )
	{
		SInt64 result;
		
		OSErr err = FSGetForkSize( refnum, &result );
		
		return err ? err : result;
	}
};

}
}

#endif

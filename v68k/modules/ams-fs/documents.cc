/*
	documents.cc
	------------
*/

#include "documents.hh"

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif
#ifndef __STRINGCOMPARE__
#include <StringCompare.h>
#endif

// Standard C
#include <errno.h>

// Standard C++
#include <algorithm>

// ams-common
#include "callouts.hh"
#include "FCB.hh"
#include "module_A4.hh"
#include "scoped_zone.hh"

// ams-fs
#include "freemount.hh"
#include "mount.hh"


#define STRLEN( s )      (sizeof "" s - 1)
#define STR_LEN( s )  s, (sizeof "" s - 1)

#define GETFINFO "..namedfork/GetFInfo"
#define SETFINFO "..namedfork/SetFInfo"


short SFSaveDisk : 0x0214;

int docfs_fd;

void mount_virtual_documents_volume()
{
	static VCB virtual_documents_volume_VCB;
	
	VCB* vcb = &virtual_documents_volume_VCB;
	
	fast_memset( vcb, '\0', sizeof (VCB) );
	
	vcb->vcbSigWord = 0xD0C5;
	vcb->vcbFSID    = 0xD0C5;
	
	// 16 MiB free should be sufficient for now
	vcb->vcbAlBlkSiz = 4096;
	vcb->vcbFreeBks  = 4096;
	
	#define VOLNAME  "\p" "Documents"
	
	fast_memcpy( vcb->vcbVN, VOLNAME, sizeof VOLNAME );
	
	#undef VOLNAME
	
	const short vRefNum = mount_VCB( vcb );
	
	SFSaveDisk = -vRefNum;
}

static plus::var_string filename_cache;

struct cached_filename
{
	Str63 name;
};

static inline
bool operator<( const cached_filename& a, const cached_filename& b )
{
	return RelString( a.name, b.name, false, true ) < 0;
}

static
void sort_filenames( plus::var_string& cache )
{
	const size_t n = cache.size() / dirent_size;
	
	cached_filename* begin = (cached_filename*) &cache[ 0 ];
	cached_filename* end   = begin + n;
	
	std::sort( begin, end );
}

const uint8_t* documents_get_nth( VCB* vcb, short n )
{
	if ( n == 1 )
	{
		temp_A4 a4;
		
		int err = try_to_list( docfs_fd, plus::string::null, filename_cache );
		
		sort_filenames( filename_cache );
	}
	
	const size_t offset = (n - 1) * dirent_size;
	
	if ( offset < filename_cache.size() )
	{
		return (const uint8_t*) &filename_cache[ offset ];
	}
	
	return NULL;  // not yet implemented
}

const uint8_t* documents_lookup( VCB* vcb, const uint8_t* name )
{
	if ( name == NULL  ||  name[ 0 ] == '\0' )
	{
		return NULL;
	}
	
	return name;
}

OSErr documents_Close( FCB* fcb )
{
	Handle h = (Handle) fcb->fcbFlNum;
	
	DisposeHandle( h );
	
	return noErr;
}

OSErr documents_Write( FCB* fcb, const char* buffer, uint32_t length )
{
	temp_A4 a4;
	
	StringPtr p = *(StringHandle) fcb->fcbFlNum;
	
	plus::string name( p,              plus::delete_never );
	plus::string data( buffer, length, plus::delete_never );
	
	int err = try_to_write( docfs_fd, name, data, fcb->fcbCrPs );
	
	return err ? ioErr : noErr;
}

OSErr documents_Create( VCB* vcb, const uint8_t* name )
{
	temp_A4 a4;
	
	int err = try_to_put( docfs_fd, name, plus::string::null );
	
	return err ? ioErr : noErr;
}

enum
{
	_PtrToHand = 0xA9E3,
};

static inline asm
StringHandle PtrToHand( const void* p : __A0, UInt32 n : __D0 )
{
	_PtrToHand
}

OSErr documents_open_fork( short trap_word, FCB* fcb, const uint8_t* name )
{
	const Byte is_rsrc = trap_word;  // Open is A000, OpenRF is A00A
	
	if ( is_rsrc )
	{
		return extFSErr;
	}
	
	temp_A4 a4;
	
	plus::var_string file_data;
	
	int err = try_to_get( docfs_fd, name, file_data );
	
	if ( ! err )
	{
		scoped_zone null_zone;
		
		const size_t size = file_data.size();
		
		StringHandle h = PtrToHand( name, 1 + name[ 0 ] );
		
		fcb->fcbFlNum  = (long) h;
		
		fcb->fcbTypByt = 0;
		fcb->fcbSBlk   = 0;
		fcb->fcbEOF    = size;
		fcb->fcbPLen   = size;
		
		fcb->fcbBfAdr  = NewPtr( size );
		
		fast_memcpy( fcb->fcbBfAdr, file_data.data(), size );
	}
	
	return noErr;
}

OSErr documents_FlushFile( FCB* fcb )
{
	temp_A4 a4;
	
	StringPtr p = *(StringHandle) fcb->fcbFlNum;
	
	plus::string name( p,                           plus::delete_never );
	plus::string data( fcb->fcbBfAdr, fcb->fcbPLen, plus::delete_never );
	
	int err = try_to_put( docfs_fd, name, data );
	
	return err ? ioErr : noErr;
}

OSErr documents_GetFileInfo( HFileParam* pb, const uint8_t* name )
{
	temp_A4 a4;
	
	Size len = *name++;
	
	if ( len  &&  *name == ':' )
	{
		++name;
		--len;
	}
	
	char path[ 256 + STRLEN( "/" GETFINFO ) ] = {};
	
	fast_memcpy( path, name, len );
	fast_memcpy( path + len, STR_LEN( "/" GETFINFO ) );
	
	plus::string pathname( path, len + STRLEN( "/" GETFINFO ) );
	
	plus::var_string file_info;
	
	int err = try_to_get( docfs_fd, pathname, file_info );
	
	if ( err < 0 )
	{
		// TODO:  Check for other errors.
		return fnfErr;
	}
	
	const Size size = sizeof (FileParam) - offsetof( FileParam, ioFlAttrib );
	
	if ( file_info.size() != size )
	{
		return ioErr;
	}
	
	if ( pb->ioNamePtr )
	{
		--name;
		
		fast_memcpy( pb->ioNamePtr, name, 1 + name[ 0 ] );
	}
	
	pb->ioFRefNum = 0;
	
	fast_memcpy( &pb->ioFlAttrib, file_info.data(), size );
	
	return noErr;
}

OSErr documents_SetFileInfo( HFileParam* pb, const uint8_t* name )
{
	temp_A4 a4;
	
	Size len = *name++;
	
	if ( len  &&  *name == ':' )
	{
		++name;
		--len;
	}
	
	char path[ 256 + STRLEN( "/" SETFINFO ) ];
	
	fast_memcpy( path, name, len );
	fast_memcpy( path + len, STR_LEN( "/" SETFINFO ) );
	
	plus::string pathname( path, len + STRLEN( "/" SETFINFO ) );
	
	const Size size = sizeof (FileParam) - offsetof( FileParam, ioFlFndrInfo );
	
	plus::string file_info( (char*) &pb->ioFlFndrInfo, size );
	
	int err = try_to_put( docfs_fd, pathname, file_info );
	
	if ( err == -ENOENT )
	{
		return fnfErr;
	}
	
	if ( err )
	{
		return ioErr;
	}
	
	return noErr;
}

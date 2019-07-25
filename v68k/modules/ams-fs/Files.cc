/*
	Files.cc
	--------
*/

#include "Files.hh"

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// Standard C
#include <errno.h>
#include <stddef.h>

// ams-common
#include "callouts.hh"
#include "FCB.hh"
#include "module_A4.hh"

// ams-fs
#include "freemount.hh"
#include "MFS.hh"
#include "Volumes.hh"


enum
{
	kHFSFlagMask = 0x0200,
};

QHdr VCBQHdr : 0x0356;

Open_ProcPtr old_Open;
IO_ProcPtr   old_Close;
IO_ProcPtr   old_Read;
IO_ProcPtr   old_Write;

struct fork_spec
{
	uint16_t  forkStBlk;
	uint32_t  forkLgLen;
	uint32_t  forkPyLen;
};

static inline
const fork_spec& get_fork( const mfs::file_directory_entry* entry, Byte rsrc )
{
	return (const fork_spec&) (rsrc ? entry->flRStBlk : entry->flStBlk);
}

static inline
short FCB_index( const FCB* fcb )
{
	return fcb - FCBSPtr->fcbs + 1;
}

static
FCB* find_FCB( long fileNum )
{
	FCB* begin = FCBSPtr->fcbs;
	FCB* end   = begin + kFCBCount;
	
	for ( FCB* it = begin;  it < end;  ++it )
	{
		if ( it->fcbFlNum == fileNum )
		{
			return it;
		}
	}
	
	return NULL;
}

static inline
FCB* find_next_empty_FCB()
{
	return find_FCB( 0 );
}

void initialize()
{
	FCBSPtr = (FCBS*) NewPtr( sizeof (FCBS) );
	
	fast_memset( FCBSPtr, '\0', sizeof (FCBS) );
	
	FCBSPtr->bufSize = sizeof (FCBS);
}

short Create_patch( short trap_word : __D1, FileParam* pb : __A0 )
{
	return pb->ioResult = extFSErr;
}

static
short open_fork( short trap_word : __D1, IOParam* pb : __A0 )
{
	StringPtr name = pb->ioNamePtr;
	
	if ( ! name )
	{
		return pb->ioResult = bdNamErr;
	}
	
	FCB* fcb = find_next_empty_FCB();
	
	if ( fcb == NULL )
	{
		return pb->ioResult = tmfoErr;
	}
	
	const Byte is_rsrc = trap_word;  // Open is A000, OpenRF is A00A
	
	VCB* vcb = (VCB*) VCBQHdr.qHead;
	
	while ( vcb != NULL )
	{
		if ( const mfs::file_directory_entry* entry = MFS_lookup( vcb, name ) )
		{
			const fork_spec& fork = get_fork( entry, is_rsrc );
			
			const size_t len = fork.forkPyLen;
			
			Ptr buffer = NewPtr( len );
			
			if ( buffer == NULL )
			{
				return pb->ioResult = memFullErr;
			}
			
			MFS_load( vcb, fork.forkStBlk, buffer, len / 512 );
			
			/*
				is_rsrc is either 0x00 or 0x0A.  kioFCBResourceMask is 0x0200,
				so masking will produce either 0x00 or 0x02.
			*/
			
			fcb->fcbFlNum  = entry->flNum;
			fcb->fcbMdRByt = is_rsrc & (kioFCBResourceMask >> 8);  // see above
			fcb->fcbTypByt = entry->flVersNum;
			fcb->fcbSBlk   = fork.forkStBlk;
			fcb->fcbEOF    = fork.forkLgLen;
			fcb->fcbPLen   = fork.forkPyLen;
			fcb->fcbCrPs   = 0;
			fcb->fcbVPtr   = vcb;
			fcb->fcbBfAdr  = buffer;
			
			pb->ioRefNum = FCB_index( fcb );
			
			return pb->ioResult = noErr;
		}
		
		vcb = (VCB*) vcb->qLink;
	}
	
	temp_A4 a4;
	
	size_t len = name[ 0 ];
	
	char path[ 256 + 5 ];
	
	fast_memcpy( path, name + 1, len );
	path[ len ] = '\0';
	
	plus::var_string file_data;
	
	int err;
	
	if ( ! is_rsrc )
	{
		err = try_to_get( path, len, file_data );
		
		if ( err == -EISDIR )
		{
			fast_memcpy( path + len, "/data", 5 );
			
			err = try_to_get( path, len + 5, file_data );
		}
	}
	else
	{
		fast_memcpy( path + len, "/rsrc", 5 );
		
		err = try_to_get( path, len + 5, file_data );
	}
	
	if ( err < 0 )
	{
		return pb->ioResult = fnfErr;  // TODO:  Check for other errors.
	}
	
	const size_t size = file_data.size();
	
	if ( void* buffer = NewPtr( size ) )
	{
		BlockMoveData( file_data.data(), buffer, size );
		
		fcb->fcbFlNum = -1;  // Claim the FCB as in use.
		
		fcb->fcbEOF  =
		fcb->fcbPLen = size;
		fcb->fcbCrPs = 0;
		
		fcb->fcbBfAdr = (Ptr) buffer;
		
		pb->ioRefNum = FCB_index( fcb );
		
		return pb->ioResult = noErr;
	}
	
	return pb->ioResult = memFullErr;
}

short Open_patch( short trap_word : __D1, IOParam* pb : __A0 )
{
	if ( trap_word & kHFSFlagMask )
	{
		// not a driver
	}
	else if ( pb->ioNamePtr  &&  *pb->ioNamePtr  &&  pb->ioNamePtr[ 1 ] == '.' )
	{
		// maybe a driver
		
		return old_Open( trap_word, (FileParam*) pb );
	}
	
	return open_fork( trap_word, pb );
}

short OpenRF_patch( short trap_word : __D1, IOParam* pb : __A0 )
{
	return open_fork( trap_word, pb );
}

static inline
unsigned long min( unsigned long a, unsigned long b )
{
	return b < a ? b : a;
}

short Read_patch( short trap_word : __D1, IOParam* pb : __A0 )
{
	if ( pb->ioRefNum < 0 )
	{
		// it's a driver
		
		return old_Read( trap_word, pb );
	}
	
	if ( pb->ioReqCount < 0 )
	{
		return pb->ioResult = paramErr;  // Negative ioReqCount
	}
	
	FCB* fcb = get_FCB( pb->ioRefNum );
	
	if ( ! fcb )
	{
		return pb->ioResult = rfNumErr;
	}
	
	const short mode = pb->ioPosMode;
	
	const size_t eof = fcb->fcbEOF;
	
	long& mark = fcb->fcbCrPs;
	
	switch ( pb->ioPosMode )
	{
		case fsAtMark:
			break;
		
		case fsFromStart:
			mark = pb->ioPosOffset;
			break;
		
		case fsFromLEOF:
			mark = eof + pb->ioPosOffset;
			break;
		
		case fsFromMark:
			mark += pb->ioPosOffset;
			break;
		
		default:
			return pb->ioResult = paramErr;
	}
	
	pb->ioActCount = 0;
	
	if ( pb->ioPosOffset < eof )
	{
		long count = min( pb->ioReqCount, eof - pb->ioPosOffset );
		
		fast_memcpy( pb->ioBuffer, &fcb->fcbBfAdr[ mark ], count );
		
		pb->ioActCount = count;
		pb->ioPosOffset = mark += count;
	}
	
	return pb->ioResult = pb->ioActCount == pb->ioReqCount ? noErr : eofErr;
}

short Write_patch( short trap_word : __D1, IOParam* pb : __A0 )
{
	if ( pb->ioRefNum < 0 )
	{
		// it's a driver
		
		return old_Write( trap_word, pb );
	}
	
	return pb->ioResult = rfNumErr;
}

short GetFPos_patch( short trap_word : __D1, IOParam* pb : __A0 )
{
	pb->ioReqCount = 0;
	pb->ioActCount = 0;
	pb->ioPosMode  = 0;
	
	if ( FCB* fcb = get_FCB( pb->ioRefNum ) )
	{
		pb->ioPosOffset = fcb->fcbCrPs;
		
		return pb->ioResult = noErr;
	}
	
	return pb->ioResult = rfNumErr;
}

short SetFPos_patch( short trap_word : __D1, IOParam* pb : __A0 )
{
	FCB* fcb = get_FCB( pb->ioRefNum );
	
	if ( ! fcb )
	{
		return pb->ioResult = rfNumErr;
	}
	
	const size_t eof = fcb->fcbEOF;
	
	long& mark = fcb->fcbCrPs;
	
	switch ( pb->ioPosMode )
	{
		case fsAtMark:
			break;
		
		case fsFromStart:
			mark = pb->ioPosOffset;
			break;
		
		case fsFromLEOF:
			mark = eof + pb->ioPosOffset;
			break;
		
		case fsFromMark:
			mark += pb->ioPosOffset;
			break;
		
		default:
			return pb->ioResult = paramErr;
	}
	
	pb->ioPosOffset = mark;
	
	return pb->ioResult = noErr;
}

short GetEOF_patch( short trap_word : __D1, IOParam* pb : __A0 )
{
	if ( FCB* fcb = get_FCB( pb->ioRefNum ) )
	{
		pb->ioMisc = (Ptr) fcb->fcbEOF;
		
		return pb->ioResult = noErr;
	}
	
	return pb->ioResult = rfNumErr;
}

short Close_patch( short trap_word : __D1, IOParam* pb : __A0 )
{
	if ( pb->ioRefNum < 0 )
	{
		// it's a driver
		
		return old_Close( trap_word, pb );
	}
	
	if ( FCB* fcb = get_FCB( pb->ioRefNum ) )
	{
		DisposePtr( fcb->fcbBfAdr );
		
		fcb->fcbFlNum = 0;
		
		return pb->ioResult = noErr;
	}
	
	return pb->ioResult = rfNumErr;
}

short GetFileInfo_patch( short trap_word : __D1, FileParam* pb : __A0 )
{
	VCB* vcb = VCB_lookup( pb->ioVRefNum );
	
	if ( vcb == NULL )
	{
		return pb->ioResult = nsvErr;
	}
	
	StringPtr name = pb->ioNamePtr;
	
	if ( ! name  &&  pb->ioFDirIndex <= 0 )
	{
		return pb->ioResult = bdNamErr;
	}
	
	const mfs::file_directory_entry* entry = NULL;
	
	if ( pb->ioFDirIndex > 0 )
	{
		short index = pb->ioFDirIndex;
		
		do
		{
			entry = MFS_iterate( vcb, entry );
		}
		while ( entry != NULL  &&  --index );
	}
	else
	{
		entry = MFS_lookup( vcb, name );
	}
	
	if ( entry  &&  pb->ioNamePtr )
	{
		ConstStr255Param name = entry->flNam;
		
		fast_memcpy( pb->ioNamePtr, name, 1 + name[ 0 ] );
	}
	
	if ( entry == NULL )
	{
		return pb->ioResult = fnfErr;
	}
	
	pb->ioFRefNum = 0;  // FIXME
	
	const size_t n = offsetof( mfs::_fde, flNam );
	
	fast_memcpy( &pb->ioFlAttrib, entry, n );
	
	return pb->ioResult = noErr;
}

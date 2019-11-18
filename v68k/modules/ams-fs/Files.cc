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

// log-of-war
#include "logofwar/report.hh"

// ams-common
#include "callouts.hh"
#include "FCB.hh"

// ams-fs
#include "appfs.hh"
#include "bootstrap.hh"
#include "MFS.hh"
#include "Volumes.hh"


enum
{
	kHFSFlagMask = 0x0200,
};

QHdr VCBQHdr : 0x0356;

short CurApRefNum : 0x0900;

Open_ProcPtr old_Open;
IO_ProcPtr   old_Close;
IO_ProcPtr   old_Read;
IO_ProcPtr   old_Write;

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

static inline
bool is_current_application( uint32_t flNum )
{
	const FCB& curApFCB = FCBSPtr->fcbs[ CurApRefNum - 1 ];
	
	return curApFCB.fcbFlNum == flNum;
}

static inline
bool is_writable( const FCB* fcb )
{
	const SInt8 flags = fcb->fcbMdRByt;
	
	return flags & (kioFCBWriteMask >> 8);
}

static inline
bool is_servable( const FCB* fcb )
{
	return fcb->fcbFlPos;
}

static inline
void set_writable( FCB* fcb )
{
	fcb->fcbMdRByt |= (kioFCBWriteMask >> 8);
}

static inline
void set_servable( FCB* fcb )
{
	fcb->fcbFlPos = 1;
}

void initialize()
{
	FCBSPtr = (FCBS*) NewPtr( sizeof (FCBS) );
	
	fast_memset( FCBSPtr, '\0', sizeof (FCBS) );
	
	FCBSPtr->bufSize = sizeof (FCBS);
}

short Create_patch( short trap_word : __D1, FileParam* pb : __A0 )
{
	return pb->ioResult = wPrErr;
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
	
	/*
		Self-modification applies to the application's own data fork,
		when write permission is requested and an appfs server is present.
	*/
	
	const bool selfmod_capable = ! is_rsrc  &&  pb->ioPermssn > 1  &&  appfs_fd;
	
	VCB* vcb = (VCB*) VCBQHdr.qHead;
	
	for ( ;  vcb != NULL;  vcb = (VCB*) vcb->qLink )
	{
		if ( const mfs::file_directory_entry* entry = MFS_lookup( vcb, name ) )
		{
			/*
				is_rsrc is either 0x00 or 0x0A.  kioFCBResourceMask is 0x0200,
				so masking will produce either 0x00 or 0x02.
			*/
			
			fcb->fcbMdRByt = is_rsrc & (kioFCBResourceMask >> 8);  // see above
			
			fcb->fcbCrPs   = 0;
			fcb->fcbVPtr   = vcb;
			
			fcb->fcbFlPos  = 0;
			
			MFS_open_fork( trap_word, fcb, entry );
			
			pb->ioRefNum = FCB_index( fcb );
			
			if ( selfmod_capable  &&  is_current_application( fcb->fcbFlNum ) )
			{
				set_writable ( fcb );  // writing is allowed
				set_servable ( fcb );  // file persists via appfs
				load_app_data( fcb );  // try to read from appfs
			}
			
			return pb->ioResult = noErr;
		}
	}
	
	return pb->ioResult = bootstrap_open_fork( trap_word, fcb, name );
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

static
OSErr adjust_mark( long& mark, const IOParam* pb : __A0, long eof )
{
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
			return paramErr;
	}
	
	return noErr;
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
	
	const size_t eof = fcb->fcbEOF;
	
	long& mark = fcb->fcbCrPs;
	
	if ( OSErr err = adjust_mark( mark, pb, eof ) )
	{
		return pb->ioResult = err;
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
	
	if ( pb->ioReqCount < 0 )
	{
		return pb->ioResult = paramErr;  // Negative ioReqCount
	}
	
	FCB* fcb = get_FCB( pb->ioRefNum );
	
	if ( ! fcb )
	{
		return pb->ioResult = rfNumErr;
	}
	
	if ( ! is_writable( fcb ) )
	{
		return pb->ioResult = wrPermErr;
	}
	
	const size_t eof = fcb->fcbEOF;
	
	long& mark = fcb->fcbCrPs;
	
	if ( OSErr err = adjust_mark( mark, pb, eof ) )
	{
		return pb->ioResult = err;
	}
	
	pb->ioActCount = 0;
	
	if ( pb->ioPosOffset < eof )
	{
		long count = min( pb->ioReqCount, eof - pb->ioPosOffset );
		
		fast_memcpy( &fcb->fcbBfAdr[ mark ], pb->ioBuffer, count );
		
		pb->ioActCount = count;
		pb->ioPosOffset = mark += count;
		
		if ( is_servable( fcb ) )
		{
			if ( OSErr err = save_app_data( fcb ) )
			{
				return err;
			}
		}
	}
	
	return pb->ioResult = pb->ioActCount == pb->ioReqCount ? noErr : eofErr;
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
	
	if ( OSErr err = adjust_mark( mark, pb, eof ) )
	{
		return pb->ioResult = err;
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
	
	const mfs::file_directory_entry* entry = NULL;
	
	if ( pb->ioFDirIndex > 0 )
	{
		entry = MFS_get_nth( vcb, pb->ioFDirIndex );
	}
	else if ( StringPtr name = pb->ioNamePtr )
	{
		entry = MFS_lookup( vcb, name );
	}
	else
	{
		return pb->ioResult = bdNamErr;
	}
	
	if ( entry == NULL )
	{
		return pb->ioResult = fnfErr;
	}
	
	pb->ioFRefNum = 0;  // FIXME
	
	return pb->ioResult = MFS_GetFileInfo( pb, entry );
}

static
OSErr GetWDInfo_call( WDPBRec* pb : __A0 )
{
	const short vRefNum = pb->ioVRefNum;
	
	if ( vRefNum >= 0 )
	{
		ERROR = "GetWDInfo got a non-negative vRefNum: ", vRefNum;
		
		return pb->ioResult = paramErr;
	}
	
	if ( pb->ioNamePtr )
	{
		if ( VCB* vcb = VCB_lookup( vRefNum ) )
		{
			const uint8_t* name = vcb->vcbVN;
			
			fast_memcpy( pb->ioNamePtr, name, 1 + name[ 0 ] );
		}
	}
	
	pb->ioWDProcID  = 0;
	pb->ioWDVRefNum = vRefNum;
	pb->ioWDDirID   = fsRtDirID;
	
	return pb->ioResult = noErr;
}

static
OSErr GetFCBInfo_call( FCBPBRec* pb : __A0 )
{
	ERROR = "GetFCBInfo is unimplemented";
	
	return paramErr;
}

static
void unimplemented_call( short trap_word : __D1, short selector : __D0 )
{
	const char* FSDispatch = "HFSDispatch" + !(trap_word & kHFSFlagMask);
	
	FATAL = "unimplemented ", FSDispatch, " call ", selector;
	
	asm { ILLEGAL }
}

asm void FSDispatch_patch( short trap_word : __D1, short selector : __D0 )
{
	CMPI.W   #0x0007,D0
	BEQ      dispatch_GetWDInfo
	
	CMPI.W   #0x0008,D0
	BEQ      dispatch_GetFCBInfo
	
	JMP      unimplemented_call
	
dispatch_GetWDInfo:
	JMP      GetWDInfo_call
	
dispatch_GetFCBInfo:
	JMP      GetFCBInfo_call
}

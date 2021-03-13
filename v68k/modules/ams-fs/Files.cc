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
#include "c_string.hh"
#include "FCB.hh"
#include "scoped_zone.hh"

// ams-fs
#include "appfs.hh"
#include "vfs.hh"
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
	return (Ptr) fcb - (Ptr) FCBSPtr;
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
	const FCB& curApFCB = *get_FCB( CurApRefNum );
	
	return curApFCB.fcbFlNum == flNum;
}

static inline
bool is_writable( const FCB* fcb )
{
	const SInt8 flags = fcb->fcbMdRByt;
	
	return flags & (kioFCBWriteMask >> 8);
}

static inline
bool is_locked( const FCB* fcb )
{
	const SInt8 flags = fcb->fcbMdRByt;
	
	return flags & (kioFCBFileLockedMask >> 8);
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

static
OSErr volume_lock_error( const VCB* vcb )
{
	if ( vcb->vcbAtrb & kioVAtrbSoftwareLockedMask )
	{
		return vLckdErr;
	}
	
	if ( vcb->vcbAtrb & kioVAtrbHardwareLockedMask )
	{
		return wPrErr;
	}
	
	return noErr;
}

short Create_patch( short trap_word : __D1, HFileParam* pb : __A0 )
{
	const short is_HFS = trap_word & kHFSFlagMask;
	
	INFO = "HCreate:" + ! is_HFS;
	INFO = "-> ioVRefNum: ", pb->ioVRefNum;
	
	if ( is_HFS )
	{
		INFO = "-> ioDirID:   ", pb->ioDirID;
	}
	
	if ( pb->ioNamePtr )
	{
		INFO = "-> ioNamePtr: \"", CSTR( pb->ioNamePtr ), "\"";
	}
	
	VCB* vcb = VCB_lookup( pb->ioVRefNum );
	
	if ( vcb == NULL )
	{
		return pb->ioResult = nsvErr;
	}
	
	DEBUG = "on volume \"", CSTR( vcb->vcbVN ), "\"";
	
	if ( OSErr err = volume_lock_error( vcb ) )
	{
		WARNING = "volume \"", CSTR( vcb->vcbVN ), "\" is locked: ", err;
		
		return pb->ioResult = err;
	}
	
	const vfs_table* vfs = vfs_from_vcb( vcb );
	
	if ( vfs == NULL )
	{
		return pb->ioResult = extFSErr;
	}
	
	return pb->ioResult = vfs->Create( vcb, pb->ioNamePtr );
}

static
short open_fork( short trap_word : __D1, HFileParam* pb : __A0 )
{
	const short is_HFS = trap_word & kHFSFlagMask;
	
	INFO = "-> ioVRefNum: ", pb->ioVRefNum;
	
	if ( is_HFS )
	{
		INFO = "-> ioDirID:   ", pb->ioDirID;
	}
	
	StringPtr name = pb->ioNamePtr;
	
	if ( ! name )
	{
		return pb->ioResult = bdNamErr;
	}
	
	INFO = "-> ioNamePtr: \"", CSTR( pb->ioNamePtr ), "\"";
	
	INFO = "-> ioPermssn: ", pb->filler1;  // ioPermssn
	
	FCB* fcb = find_next_empty_FCB();
	
	if ( fcb == NULL )
	{
		return pb->ioResult = tmfoErr;
	}
	
	const Byte is_rsrc = trap_word;  // Open is A000, OpenRF is A00A
	
	/*
		is_rsrc is either 0x00 or 0x0A.  kioFCBResourceMask is 0x0200,
		so masking will produce either 0x00 or 0x02.
	*/
	
	fcb->fcbMdRByt = is_rsrc & (kioFCBResourceMask >> 8);  // see above
	
	const bool writable = pb->filler1 != 1;  // ioPermssn
	
	if ( writable )
	{
		// 1 is read-only, 2 is write-only, 3 is both, and 0 is whatever.
		
		set_writable( fcb );  // writing is allowed
	}
	
	/*
		Self-modification applies to the application's own data fork,
		when write permission is requested and an appfs server is present.
	*/
	
	const bool selfmod_capable = ! is_rsrc  &&  writable  &&  appfs_fd;
	
	VCB* vcb = (VCB*) VCBQHdr.qHead;
	
	for ( ;  vcb != NULL;  vcb = (VCB*) vcb->qLink )
	{
		const vfs_table* vfs = vfs_from_vcb( vcb );
		
		if ( vfs == NULL )
		{
			return pb->ioResult = extFSErr;
		}
		
		if ( const generic_file_entry* entry = vfs->lookup( vcb, name ) )
		{
			DEBUG = "on volume \"", CSTR( vcb->vcbVN ), "\"";
			
			fcb->fcbCrPs   = 0;
			fcb->fcbVPtr   = vcb;
			
			fcb->fcbFlPos  = 0;
			
			if ( OSErr err = vfs->open_fork( trap_word, fcb, entry ) )
			{
				if ( err == fnfErr )
				{
					continue;
				}
				
				return pb->ioResult = err;
			}
			
			pb->ioFRefNum = FCB_index( fcb );  // ioRefNum
			
			INFO = "<- ioFRefNum: ", pb->ioFRefNum;
			
			if ( selfmod_capable  &&  is_current_application( fcb->fcbFlNum ) )
			{
				set_servable ( fcb );  // file persists via appfs
				load_app_data( fcb );  // try to read from appfs
			}
			
			return pb->ioResult = noErr;
		}
	}
	
	return pb->ioResult = fnfErr;
}

short Open_patch( short trap_word : __D1, HFileParam* pb : __A0 )
{
	const short is_HFS = trap_word & kHFSFlagMask;
	
	INFO = "HOpen:" + ! is_HFS;
	
	if ( is_HFS )
	{
		// not a driver
	}
	else if ( pb->ioNamePtr  &&  *pb->ioNamePtr  &&  pb->ioNamePtr[ 1 ] == '.' )
	{
		// maybe a driver
		
		INFO = "-> ioNamePtr: \"", CSTR( pb->ioNamePtr ), "\"";
		
		return old_Open( trap_word, (FileParam*) pb );
	}
	
	return open_fork( trap_word, pb );
}

short OpenRF_patch( short trap_word : __D1, HFileParam* pb : __A0 )
{
	const short is_HFS = trap_word & kHFSFlagMask;
	
	INFO = "HOpenRF:" + ! is_HFS;
	
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
	
	if ( mark < eof )
	{
		long count = min( pb->ioReqCount, eof - mark );
		
		fast_memcpy( pb->ioBuffer, &fcb->fcbBfAdr[ mark ], count );
		
		pb->ioActCount = count;
		pb->ioPosOffset = mark += count;
	}
	
	return pb->ioResult = pb->ioActCount == pb->ioReqCount ? noErr : eofErr;
}

static
OSErr writability_error( const FCB* fcb )
{
	if ( ! is_writable( fcb ) )
	{
		return wrPermErr;
	}
	
	if ( is_locked( fcb ) )
	{
		return fLckdErr;
	}
	
	return volume_lock_error( fcb->fcbVPtr );
}

static
OSErr flush_file( FCB* fcb )
{
	if ( is_servable( fcb ) )
	{
		return save_app_data( fcb );
	}
	
	const vfs_table* vfs = vfs_from_vcb( fcb->fcbVPtr );
	
	if ( vfs == NULL )
	{
		return extFSErr;
	}
	
	return vfs->FlushFile( fcb );
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
	
	if ( OSErr err = writability_error( fcb ) )
	{
		if ( err != wPrErr  ||  ! is_servable( fcb ) )
		{
			return pb->ioResult = err;
		}
	}
	
	const size_t eof = fcb->fcbEOF;
	
	long& mark = fcb->fcbCrPs;
	
	if ( OSErr err = adjust_mark( mark, pb, eof ) )
	{
		return pb->ioResult = err;
	}
	
	pb->ioActCount = 0;
	
	if ( mark <= eof )
	{
		long count = pb->ioReqCount;
		
		if ( count > eof - mark )
		{
			scoped_zone null_zone;
			
			const long new_eof = mark + count;
			
			Ptr buffer = NewPtr( new_eof );
			
			if ( buffer == NULL )
			{
				return pb->ioResult = ioErr;
			}
			
			fast_memcpy( buffer, fcb->fcbBfAdr, mark );
			
			DisposePtr( fcb->fcbBfAdr );
			
			fcb->fcbBfAdr = buffer;
			
			fcb->fcbEOF  = new_eof;
			fcb->fcbPLen = new_eof;
		}
		
		fast_memcpy( &fcb->fcbBfAdr[ mark ], pb->ioBuffer, count );
		
		pb->ioActCount = count;
		pb->ioPosOffset = mark += count;
		
		if ( OSErr err = flush_file( fcb ) )
		{
			return pb->ioResult = err;
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

short SetEOF_patch( short trap_word : __D1, IOParam* pb : __A0 )
{
	FCB* fcb = get_FCB( pb->ioRefNum );
	
	if ( ! fcb )
	{
		return pb->ioResult = rfNumErr;
	}
	
	if ( OSErr err = writability_error( fcb ) )
	{
		if ( err != wPrErr  ||  ! is_servable( fcb ) )
		{
			return pb->ioResult = err;
		}
	}
	
	const long new_eof = (long) pb->ioMisc;
	
	if ( new_eof > fcb->fcbEOF )
	{
		return pb->ioResult = extFSErr;
	}
	
	if ( new_eof < fcb->fcbEOF )
	{
		fcb->fcbEOF  = new_eof;
		fcb->fcbPLen = new_eof;
		
		if ( OSErr err = flush_file( fcb ) )
		{
			return pb->ioResult = err;
		}
	}
	
	return pb->ioResult = noErr;
}

short Allocate_patch( short trap_word : __D1, IOParam* pb : __A0 )
{
	FCB* fcb = get_FCB( pb->ioRefNum );
	
	if ( ! fcb )
	{
		return pb->ioResult = rfNumErr;
	}
	
	if ( OSErr err = writability_error( fcb ) )
	{
		if ( err != wPrErr  ||  ! is_servable( fcb ) )
		{
			return pb->ioResult = err;
		}
	}
	
	const long new_eof = pb->ioReqCount;
	
	WARNING = "Ignoring Allocate of ", new_eof;
	
	return pb->ioResult = noErr;
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
		if ( const vfs_table* vfs = vfs_from_vcb( fcb->fcbVPtr ) )
		{
			if ( vfs->Close )
			{
				vfs->Close( fcb );
			}
		}
		
		if ( fcb->fcbBfAdr )
		{
			DisposePtr( fcb->fcbBfAdr );
			
			fcb->fcbBfAdr = NULL;
		}
		
		fcb->fcbFlNum = 0;
		
		return pb->ioResult = noErr;
	}
	
	return pb->ioResult = rfNumErr;
}

short Delete_patch( short trap_word : __D1, HFileParam* pb : __A0 )
{
	const short is_HFS = trap_word & kHFSFlagMask;
	
	INFO = "HDelete:" + ! is_HFS;
	INFO = "-> ioVRefNum: ", pb->ioVRefNum;
	
	if ( is_HFS )
	{
		INFO = "-> ioDirID:   ", pb->ioDirID;
	}
	
	if ( pb->ioNamePtr )
	{
		INFO = "-> ioNamePtr: \"", CSTR( pb->ioNamePtr ), "\"";
	}
	
	ERROR = "Delete is unimplemented";
	
	return pb->ioResult = extFSErr;
}

short GetFileInfo_patch( short trap_word : __D1, HFileParam* pb : __A0 )
{
	const short is_HFS = trap_word & kHFSFlagMask;
	
	INFO = "HGetFileInfo:" + ! is_HFS;
	INFO = "-> ioVRefNum: ", pb->ioVRefNum;
	
	if ( is_HFS )
	{
		INFO = "-> ioDirID:   ", pb->ioDirID;
	}
	
	if ( pb->ioNamePtr )
	{
		INFO = "-> ioNamePtr: \"", CSTR( pb->ioNamePtr ), "\"";
	}
	
	VCB* vcb = VCB_lookup( pb->ioVRefNum );
	
	if ( vcb == NULL )
	{
		return pb->ioResult = nsvErr;
	}
	
	DEBUG = "on volume \"", CSTR( vcb->vcbVN ), "\"";
	
	const vfs_table* vfs = vfs_from_vcb( vcb );
	
	if ( vfs == NULL )
	{
		return pb->ioResult = extFSErr;
	}
	
	const generic_file_entry* entry = NULL;
	
	if ( pb->ioFDirIndex > 0 )
	{
		entry = vfs->get_nth( vcb, pb->ioFDirIndex );
	}
	else if ( StringPtr name = pb->ioNamePtr )
	{
		entry = vfs->lookup( vcb, name );
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
	
	return pb->ioResult = vfs->GetFileInfo( pb, entry );
}

short SetFileInfo_patch( short trap_word : __D1, HFileParam* pb : __A0 )
{
	const short is_HFS = trap_word & kHFSFlagMask;
	
	INFO = "HSetFileInfo:" + ! is_HFS;
	INFO = "-> ioVRefNum: ", pb->ioVRefNum;
	
	if ( is_HFS )
	{
		INFO = "-> ioDirID:   ", pb->ioDirID;
	}
	
	if ( pb->ioNamePtr )
	{
		INFO = "-> ioNamePtr: \"", CSTR( pb->ioNamePtr ), "\"";
	}
	
	VCB* vcb = VCB_lookup( pb->ioVRefNum );
	
	if ( vcb == NULL )
	{
		return pb->ioResult = nsvErr;
	}
	
	DEBUG = "on volume \"", CSTR( vcb->vcbVN ), "\"";
	
	if ( OSErr err = volume_lock_error( vcb ) )
	{
		WARNING = "volume \"", CSTR( vcb->vcbVN ), "\" is locked: ", err;
		
		return pb->ioResult = err;
	}
	
	const vfs_table* vfs = vfs_from_vcb( vcb );
	
	if ( vfs == NULL )
	{
		return pb->ioResult = extFSErr;
	}
	
	const generic_file_entry* entry = NULL;
	
	if ( pb->ioFDirIndex > 0 )
	{
		entry = vfs->get_nth( vcb, pb->ioFDirIndex );
	}
	else if ( StringPtr name = pb->ioNamePtr )
	{
		entry = vfs->lookup( vcb, name );
	}
	else
	{
		return pb->ioResult = bdNamErr;
	}
	
	if ( entry == NULL )
	{
		return pb->ioResult = fnfErr;
	}
	
	if ( vfs->SetFileInfo )
	{
		return pb->ioResult = vfs->SetFileInfo( pb, entry );
	}
	
	WARNING = "returning noErr for unimplemented SetFileInfo";
	
	return pb->ioResult = noErr;
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
	
	return pb->ioResult = paramErr;
}

static
OSErr OpenDF_call( IOParam* pb : __A0 )
{
	ERROR = "OpenDF is unimplemented";
	
	return pb->ioResult = paramErr;
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
	
	CMPI.W   #0x001A,D0
	BEQ      dispatch_OpenDF
	
	JMP      unimplemented_call
	
dispatch_GetWDInfo:
	JMP      GetWDInfo_call
	
dispatch_GetFCBInfo:
	JMP      GetFCBInfo_call
	
dispatch_OpenDF:
	JMP      OpenDF_call
}

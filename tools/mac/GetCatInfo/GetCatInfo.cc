/*
	GetCatInfo.cc
	-------------
*/

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// POSIX
#include <unistd.h>

// Standard C
#include <time.h>

// iota
#include "iota/endian.hh"

// mac-types
#include "mac_types/epoch.hh"

// mac-relix-utils
#include "mac_relix/FSRef_from_path.hh"
#include "mac_relix/FSSpec_from_path.hh"


#pragma exceptions off


#define PREFIX  "GetCatInfo: "

#define STR_LEN( s )  "" s, (sizeof s - 1)

#define ERROR( e, msg )  (wrote( 2, STR_LEN( PREFIX msg "\n" ) ) ? e : 13)


#if __LP64__

#pragma pack(push, 2)

struct HFileInfo
{
	SInt8   ioFlAttrib;
	SInt8   ioACUser;
	FInfo   ioFlFndrInfo;
	SInt32  ioDirID;
	UInt16  ioFlStBlk;
	SInt32  ioFlLgLen;
	SInt32  ioFlPyLen;
	UInt16  ioFlRStBlk;
	SInt32  ioFlRLgLen;
	SInt32  ioFlRPyLen;
	UInt32  ioFlCrDat;
	UInt32  ioFlMdDat;
	UInt32  ioFlBkDat;
	FXInfo  ioFlXFndrInfo;
	SInt32  ioFlParID;
	SInt32  ioFlClpSiz;
};

union CInfoPBRec
{
	HFileInfo hFileInfo;
};

#pragma pack(pop)

#endif

static inline
bool wrote( int fd, const void* buffer, size_t n )
{
	return write( fd, buffer, n ) == n;
}

static
UInt32 localize( UInt32 mac_time )
{
	if ( mac_time )
	{
		const unsigned delta = mac::types::epoch_delta();
		
		time_t c_time = mac_time - delta;
		
		tm* t = localtime( &c_time );
		
		return mac_time + t->tm_gmtoff;
	}
	
	return 0;
}

int main( int argc, char** argv )
{
	if ( argc != 2 )
	{
		return ERROR( 50, "one path argument required" );
	}
	
	const char* target_path = argv[ 1 ];
	
#if __LP64__
	
	typedef FSRef FSObj;
	
#else
	
	typedef FSSpec FSObj;
	
#endif
	
	FSObj target_filespec;
	
	if ( mac::relix::FSObj_from_existing_path( target_path, target_filespec ) )
	{
		return 43;  // producing an FSObj failed; presumably nonexistent
	}
	
	CInfoPBRec cInfo;
	
	HFileInfo& pb = cInfo.hFileInfo;
	
	/*
		Zero ioACUser in advance, in case a file system doesn't
		set it (32-bit), or because we set everything (64-bit).
		
		From Files.h:  "Note: you must clear ioACUser before calling
		PBGetCatInfo because some file systems do not use this field"
	*/
	
	pb.ioACUser = 0;
	
#if __LP64__
	
	const FSCatalogInfoBitmap bitmap_for_HFileInfo
		= kFSCatInfoNodeFlags
		| kFSCatInfoParentDirID
		| kFSCatInfoNodeID
		| kFSCatInfoCreateDate
		| kFSCatInfoContentMod
		| kFSCatInfoBackupDate
		| kFSCatInfoFinderInfo
		| kFSCatInfoFinderXInfo
		| kFSCatInfoDataSizes
		| kFSCatInfoRsrcSizes;
	
	FSCatalogInfoBitmap bits = bitmap_for_HFileInfo;
	FSCatalogInfo       info;
	
	OSStatus err = FSGetCatalogInfo( &target_filespec, bits, &info, 0, 0, 0 );
	
	if ( err )
	{
		return ERROR( 36, "can't get catalog info" );
	}
	
	pb.ioFlAttrib = info.nodeFlags;
	
	pb.ioFlFndrInfo = *(const FInfo*) &info.finderInfo;
	
	pb.ioDirID = info.nodeID;
	pb.ioFlStBlk = 0;
	pb.ioFlLgLen = info.dataLogicalSize;
	pb.ioFlPyLen = info.dataPhysicalSize;
	pb.ioFlRStBlk = 0;
	pb.ioFlRLgLen = info.rsrcLogicalSize;
	pb.ioFlRPyLen = info.rsrcPhysicalSize;
	pb.ioFlCrDat = localize( info.createDate    .lowSeconds );
	pb.ioFlMdDat = localize( info.contentModDate.lowSeconds );
	pb.ioFlBkDat = localize( info.backupDate    .lowSeconds );
	
	pb.ioFlXFndrInfo = *(const FXInfo*) &info.extFinderInfo;
	
	pb.ioFlParID  = info.parentDirID;
	pb.ioFlClpSiz = 0;
	
#else
	
	pb.ioNamePtr   = target_filespec.name;
	pb.ioVRefNum   = target_filespec.vRefNum;
	pb.ioFDirIndex = 0;
	pb.ioDirID     = target_filespec.parID;
	
	OSErr err = PBGetCatInfoSync( &cInfo );
	
	if ( err )
	{
		return ERROR( 36, "can't get catalog info" );
	}
	
#endif
	
	if ( isatty( STDOUT_FILENO ) )
	{
		return ERROR( 50, "please redirect output to a file" );
	}
	
	if ( iota::is_little_endian() )
	{
		FInfo& info = pb.ioFlFndrInfo;
		
		info.fdType       = iota::big_u32( info.fdType       );
		info.fdCreator    = iota::big_u32( info.fdCreator    );
		info.fdFlags      = iota::big_u16( info.fdFlags      );
		info.fdLocation.v = iota::big_u16( info.fdLocation.v );
		info.fdLocation.h = iota::big_u16( info.fdLocation.h );
		info.fdFldr       = iota::big_u16( info.fdFldr       );
		
		FXInfo& xinfo = pb.ioFlXFndrInfo;
		
		xinfo.fdIconID        = iota::big_u16( xinfo.fdIconID        );
		xinfo.fdReserved[ 0 ] = iota::big_u16( xinfo.fdReserved[ 0 ] );
		xinfo.fdReserved[ 1 ] = iota::big_u16( xinfo.fdReserved[ 1 ] );
		xinfo.fdReserved[ 2 ] = iota::big_u16( xinfo.fdReserved[ 2 ] );
		xinfo.fdComment       = iota::big_u16( xinfo.fdComment       );
		xinfo.fdPutAway       = iota::big_u32( xinfo.fdPutAway       );
		
		pb.ioDirID    = iota::big_u32( pb.ioDirID    );
		pb.ioFlStBlk  = iota::big_u16( pb.ioFlStBlk  );
		pb.ioFlLgLen  = iota::big_u32( pb.ioFlLgLen  );
		pb.ioFlPyLen  = iota::big_u32( pb.ioFlPyLen  );
		pb.ioFlRStBlk = iota::big_u16( pb.ioFlRStBlk );
		pb.ioFlRLgLen = iota::big_u32( pb.ioFlRLgLen );
		pb.ioFlRPyLen = iota::big_u32( pb.ioFlRPyLen );
		pb.ioFlCrDat  = iota::big_u32( pb.ioFlCrDat  );
		pb.ioFlMdDat  = iota::big_u32( pb.ioFlMdDat  );
		pb.ioFlBkDat  = iota::big_u32( pb.ioFlBkDat  );
		pb.ioFlParID  = iota::big_u32( pb.ioFlParID  );
		pb.ioFlClpSiz = iota::big_u32( pb.ioFlClpSiz );
	}
	
	size_t size = sizeof pb - offsetof( HFileInfo, ioFlAttrib );
	
	write( STDOUT_FILENO, &pb.ioFlAttrib, size );
	
	return 0;
}

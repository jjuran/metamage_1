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

// iota
#include "iota/endian.hh"

// mac-relix-utils
#include "mac_relix/FSSpec_from_path.hh"


#pragma exceptions off


#define PREFIX  "GetCatInfo: "

#define STR_LEN( s )  "" s, (sizeof s - 1)

#define ERROR( e, msg )  (wrote( 2, STR_LEN( PREFIX msg "\n" ) ) ? e : 13)


static inline
bool wrote( int fd, const void* buffer, size_t n )
{
	return write( fd, buffer, n ) == n;
}

int main( int argc, char** argv )
{
	if ( argc != 2 )
	{
		return ERROR( 50, "one path argument required" );
	}
	
	const char* target_path = argv[ 1 ];
	
	FSSpec target_filespec;
	
	if ( mac::relix::FSSpec_from_existing_path( target_path, target_filespec ) )
	{
		return 43;  // producing an FSSpec failed; presumably nonexistent
	}
	
	CInfoPBRec cInfo;
	
	HFileInfo& pb = cInfo.hFileInfo;
	
	/*
		Zero ioACUser in advance, in case a file system doesn't set it.
		
		From Files.h:  "Note: you must clear ioACUser before calling
		PBGetCatInfo because some file systems do not use this field"
	*/
	
	pb.ioACUser = 0;
	
	pb.ioNamePtr   = target_filespec.name;
	pb.ioVRefNum   = target_filespec.vRefNum;
	pb.ioFDirIndex = 0;
	pb.ioDirID     = target_filespec.parID;
	
	OSErr err = PBGetCatInfoSync( &cInfo );
	
	if ( err )
	{
		return ERROR( 36, "can't get catalog info" );
	}
	
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

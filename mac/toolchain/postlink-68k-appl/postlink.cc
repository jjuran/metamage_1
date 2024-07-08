/*
	postlink.cc
	-----------
*/

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// POSIX
#include <unistd.h>

// Standard C
#include <stdlib.h>

// iota
#include "iota/endian.hh"

// command
#include "command/get_option.hh"

// mac-relix-utils
#include "mac_relix/FSSpec_from_path.hh"


#pragma exceptions off


#define PREFIX  "postlink-68k-appl: "

#define STR_LEN( s )  "" s, (sizeof s - 1)

#define ERROR( e, msg )  (wrote( 2, STR_LEN( PREFIX msg "\n" ) ) ? e : 13)


using namespace command::constants;

enum
{
	Option_dry_run = 'n',
	Option_verbose = 'v',
};

static command::option options[] =
{
	{ "", Option_dry_run },
	{ "", Option_verbose },
	
	{ NULL }
};

static bool dry_run = false;
static bool verbose = false;

static inline
bool wrote( int fd, const void* buffer, size_t n )
{
	return write( fd, buffer, n ) == n;
}

static
char* const* get_options( char* const* argv )
{
	++argv;  // skip arg 0
	
	short opt;
	
	while ( (opt = command::get_option( &argv, options )) )
	{
		switch ( opt )
		{
			case Option_dry_run:
				dry_run = true;
				break;
			
			case Option_verbose:
				verbose = true;
				break;
			
			default:
				abort();
		}
	}
	
	return argv;
}

static inline
void nopify( UInt16* p, int n )
{
	while ( n-- > 0 )
	{
		*p++ = iota::big_u16( 0x4E71 );  // NOP
	}
}

static inline
bool patchable( const UInt16* p )
{
	p += 0x009a / 2;
	
	return *p++ == iota::big_u16( 0x7001 )  &&  // MOVEQ    #1,D0
	       *p++ == iota::big_u16( 0xA198 )  &&  // _HWPriv
	       *p++ == iota::big_u16( 0x303C )  &&  // MOVE.W   ...      ,D0
	       *p++ == iota::big_u16( 0xA9F0 )  &&  //          #_LoadSeg
	       *p++ == iota::big_u16( 0xA746 )  &&  // _GetToolBoxTrapAddress
	       true;
}

static
bool patch_CODE_1( Handle code )
{
	Ptr p = *code;
	
	if ( ! patchable( (UInt16*) p ) )
	{
		return false;
	}
	
	p += iota::big_u16( 0x0058 );
	
	// 000058:
	
	UInt16* p16 = (UInt16*) p;
	
	/*
		Drop the x-ref offset into D4 en passant.
		We assume that no relocated code lies beyond it.
	*/
	
	*p16++ = iota::big_u16( 0x2818 );  // MOVE.L   (A0)+,D4
	*p16++ = iota::big_u16( 0x4850 );  // PEA      (A0)
	
	p16 += 3;
	
	// 000062:
	
	/*
		Load the start-of-code address into A4 (not A1),
		so it won't get clobbered by subroutine calls.
	*/
	
	*p16++ = iota::big_u16( 0x49FA );  // LEA      ...,A4
	
	++p16;
	
	*p16++ = iota::big_u16( 0x2B4C );  // MOVE.L   A4,...
	
	p16 += 9;
	
	// 00007a:
	
	/*
		BlockMove the code region onto itself, which is a
		no-op with regard to memory contents, but updates
		the instruction cache, if there is one -- without
		our having to check for it.  This avoids mistakes
		that are common when checking for the presence of
		new routines in the 64K ROM's unified trap table.
	*/
	
	*p16++ = iota::big_u16( 0x204C );  // MOVEA.L  A4,A0
	*p16++ = iota::big_u16( 0x224C );  // MOVEA.L  A4,A1
	*p16++ = iota::big_u16( 0x2004 );  // MOVE.L   D4,D0
	*p16++ = iota::big_u16( 0xA02E );  // _BlockMove
	*p16++ = iota::big_u16( 0x6056 );  // BRA.S    *+88
	
	nopify( p16, 43 );
	
	return true;
}

static
Handle patch_68K_startup()
{
	short n = Count1Resources( 'CODE' );
	
	if ( n > 2 )
	{
		/*
			Don't patch multi-segment applications.  The same far
			reference support that makes it necessary for the runtime
			to patch the Segment Loader also allows having a single
			'CODE' segment, rendering those patches unneeded after all.
		*/
		
		return NULL;
	}
	
	Handle code = Get1Resource( 'CODE', 1 );
	
	if ( code )
	{
		HNoPurge( code );
		
		if ( const bool patched = patch_CODE_1( code ) )
		{
			const short attrs = GetResAttrs( code );
			
			SetResAttrs( code, attrs & ~resProtected );
			
			ChangedResource( code );
			WriteResource  ( code );
			
			SetResAttrs( code, attrs );
		}
	}
	
	return code;
}

int main( int argc, char** argv )
{
	char *const *args = get_options( argv );
	
	const int argn = argc - (args - argv);
	
	if ( argn == 0 )
	{
		ERROR( 50, "argument required" );
	}
	
	const char* target_path = args[ 0 ];
	
	FSSpec target_filespec;
	
	if ( mac::relix::FSSpec_from_existing_path( target_path, target_filespec ) )
	{
		return 43;  // producing an FSSpec failed; presumably nonexistent
	}
	
	short resfile = FSpOpenResFile( &target_filespec, fsRdWrPerm );
	
	if ( resfile <= 0 )
	{
		return 1;  // opening the resource file failed
	}
	
	Handle code = patch_68K_startup();
	
	OSErr err = noErr;
	
	CloseResFile( resfile );
	
	return err > -125 ? -err : 125;
}

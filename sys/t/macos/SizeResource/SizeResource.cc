/*
	SizeResource.cc
	---------------
	
	To run this in Advanced Mac Substitute:
	
		cd ~/src/metamage_1
		
		bin/rbuild -B46 SizeResource
		
		cd ../ams-68k-bin
		
		rm -f ~/var/tmp/SizeResource-test-tempfile.rsrc
		
		graft --fd 20  \
			freemountd --rw -qu --root ~/var/tmp/ //  \
			xv68k -St                                 \
				-mlib/ams-{core,io}              \
				-m [ lib/ams-fs --docfs-fd=20 ]  \
				-m lib/ams-rsrc                  \
				~/var/build/68k-a4-blue-dbg/bin/SizeResource/SizeResource -2
		
		rm ~/var/tmp/SizeResource-test-tempfile.rsrc
	
*/

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// gear
#include "gear/parse_decimal.hh"

// mac-sys-utils
#include "mac_sys/has/native_Carbon.hh"
#include "mac_sys/res_error.hh"

// mac-rsrc-utils
#include "mac_rsrc/create_res_file.hh"
#include "mac_rsrc/open_res_file.hh"

// tap-out
#include "tap/test.hh"


#pragma exceptions off


#define TEMPFILE_NAME  "\p" "SizeResource-test-tempfile.rsrc"


const unsigned n_tests = 22;

static short        vRefNum;
static long         dirID;
static const Byte*  name = TEMPFILE_NAME;

static short refnum;

static
void init()
{
	const OSErr noErr = 0;
	
	HDelete( vRefNum, dirID, name );
	
	OSErr err = mac::rsrc::create_res_file( vRefNum, dirID, name );
	
	EXPECT_EQ( err, noErr );
	
	refnum = mac::rsrc::open_res_file( vRefNum, dirID, name, fsRdWrPerm );
	
	err = refnum < 0 ? refnum : 0;
	
	EXPECT_EQ( err, noErr );
}

static
void added_size()
{
	using mac::sys::has_native_Carbon;
	
	const OSErr noErr        =    0;
	const OSErr nilHandleErr = -109;
	const OSErr resNotFound  = -192;
	const OSErr addResFailed = -194;
	
	if ( Handle h = NewHandle( 11 ) )
	{
		BlockMoveData( "Hello world", *h, 11 );
		
		Size size = GetHandleSize( h );
		
		EXPECT_EQ( size, 11 );
		
		SignedByte state = HGetState( h );
		
		EXPECT_EQ( state, 0x00 );
		
		size = GetResourceSizeOnDisk( h );
		
		EXPECT_EQ( size, -1 );
		
		EXPECT_EQ( mac::sys::res_error(), resNotFound );
		
		AddResource( h, 'TEXT', 128, NULL );
		
		EXPECT_EQ( mac::sys::res_error(), noErr );
		
		state = HGetState( h );
		
		EXPECT_EQ( state, 0x20 );
		
		size = GetResourceSizeOnDisk( h );
		
		EXPECT_EQ( size, 11 );
		
		SetHandleSize( h, 10 );
		
		size = GetResourceSizeOnDisk( h );
		
		EXPECT_EQ( size, 10 );
		
		WriteResource( h );
		
		size = GetResourceSizeOnDisk( h );
		
		EXPECT_EQ( size, 10 );
		
		SetHandleSize( h, 9 );
		
		size = GetResourceSizeOnDisk( h );
		
		EXPECT_EQ( size, 9 );
		
		UpdateResFile( refnum );
		
		size = GetResourceSizeOnDisk( h );
		
		EXPECT_EQ( size, 9 );
		
		SetHandleSize( h, 8 );
		
		size = GetResourceSizeOnDisk( h );
		
		EXPECT_EQ( size, 8 );
		
		EmptyHandle( h );
		
		size = GetResourceSizeOnDisk( h );
		
		EXPECT_EQ( size, 10 );
		
		DetachResource( h );
		
		state = HGetState( h );
		
		const SignedByte expected_state = has_native_Carbon() ? noErr
		                                                      : nilHandleErr;
		
		EXPECT_EQ( state, expected_state );
		
		AddResource( NULL, 'TEXT', 129, NULL );
		
		EXPECT_EQ( mac::sys::res_error(), addResFailed );
		
		AddResource( h, 'TEXT', 129, NULL );
		
		EXPECT_EQ( mac::sys::res_error(), nilHandleErr );
		
		ReallocateHandle( h, 13 );
		
		AddResource( h, 'TEXT', 129, NULL );
		
		size = GetResourceSizeOnDisk( h );
		
		EXPECT_EQ( size, 13 );
		
		EXPECT_EQ( mac::sys::res_error(), noErr );
		
		EmptyHandle( h );
		
		size = GetResourceSizeOnDisk( h );
		
		/*
			In OS X, the size returned here is zero.
			In classic Mac OS, it's garbage.
		*/
		
		EXPECT_EQ( mac::sys::res_error(), noErr );
		
		LoadResource( h );
		
		size = GetResourceSizeOnDisk( h );
		
		/*
			The size returned here is the same as above.
		*/
		
		EXPECT_EQ( mac::sys::res_error(), noErr );
	}
}

static
void term()
{
	if ( refnum > 0 )
	{
		CloseResFile( refnum );
	}
	
	HDelete( vRefNum, dirID, name );
}

int main( int argc, char** argv )
{
	tap::start( "SizeResource", n_tests );
	
	if ( argc > 1 )
	{
		vRefNum = gear::parse_decimal( argv[ 1 ] );
	}
	
	init();
	
	added_size();
	
	term();
	
	return 0;
}

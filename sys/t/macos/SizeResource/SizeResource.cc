/*
	SizeResource.cc
	---------------
*/

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// mac-sys-utils
#include "mac_sys/has/native_Carbon.hh"
#include "mac_sys/res_error.hh"

// tap-out
#include "tap/test.hh"


#pragma exceptions off


#define TEMPFILE_NAME  "\p" "SizeResource-test-tempfile.rsrc"


const unsigned n_tests = 15;

static short        vRefNum;
static long         dirID;
static const Byte*  name = TEMPFILE_NAME;

static short refnum;

static
void init()
{
#if ! TARGET_API_MAC_CARBON
	
	CreateResFile( name );
	
	refnum = OpenResFile( name );
	
	return;
	
#endif
	
	HCreateResFile( vRefNum, dirID, name );
	
	refnum = HOpenResFile( vRefNum, dirID, name, fsRdWrPerm );
}

static
void added_size()
{
	using mac::sys::has_native_Carbon;
	
	const OSErr noErr        =    0;
	const OSErr nilHandleErr = -109;
	const OSErr resNotFound  = -192;
	
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
		
		AddResource( h, 'TEXT', 129, NULL );
		
		EXPECT_EQ( mac::sys::res_error(), nilHandleErr );
		
		DisposeHandle( h );
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
	
	init();
	
	added_size();
	
	term();
	
	return 0;
}

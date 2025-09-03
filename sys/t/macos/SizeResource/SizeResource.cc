/*
	SizeResource.cc
	---------------
*/

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// mac-sys-utils
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
	CreateResFile( name );
	
	refnum = OpenResFile( name );
}

static
void added_size()
{
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
		
		EXPECT_EQ( state, nilHandleErr );
		
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

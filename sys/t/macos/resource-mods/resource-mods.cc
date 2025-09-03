/*
	resource-mods.cc
	----------------
*/

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// iota
#include "iota/endian.hh"

// gear
#include "gear/parse_decimal.hh"

// mac-sys-utils
#include "mac_sys/res_error.hh"

// tap-out
#include "tap/test.hh"


#pragma exceptions off


#define TEMPFILE_NAME  "\p" "resource-mods-test-tmpfile.rsrc"

#define ZERO_x4    "\0\0\0\0"
#define ZERO_x16   ZERO_x4  ZERO_x4  ZERO_x4  ZERO_x4
#define ZERO_x64   ZERO_x16 ZERO_x16 ZERO_x16 ZERO_x16
#define ZERO_x256  ZERO_x64 ZERO_x64 ZERO_x64 ZERO_x64

#define EMPTY_RSRC_MAP  \
	"\0\0\1\0" "\0\0\1\0" ZERO_x4        "\0\0\0\x1e"  \
	ZERO_x4    ZERO_x4    "\0\x1c\0\x1e" "\xff\xff"

#define EXPECT_CMP_DATA( offset, length, p, n )  \
	if ( read_at_pos( buffer, length, offset ) == noErr ) \
		EXPECT_CMP( buffer, length, p, n );  \
	else EXPECT( false )

#define EXPECT_ZERO_DATA( offset, length )  \
	EXPECT_CMP_DATA( offset, length, ZERO_x256, length )


const unsigned n_tests = 120;

static short        vRefNum;
static long         dirID;
static const Byte*  name = TEMPFILE_NAME;

static short refnum;

static Byte buffer[ 256 ];

typedef iota::big_endian_conversion< UInt32 >::type U_32;

struct rsrc_fork_header
{
	U_32  offset_to_data;
	U_32  offset_to_map;
	U_32  length_of_data;
	U_32  length_of_map;
};

static
long geteof()
{
	SInt32 size;
	
	OSErr err = GetEOF( refnum, &size );
	
	return err ? err : size;
}

static
OSErr read_at_pos( void* buffer, long length, long offset )
{
	OSErr err;
	Size read_size = length;
	
	(err = SetFPos( refnum, fsFromStart, offset ))  ||
	(err = FSRead ( refnum, &read_size, buffer ));
	
	return err;
}

static
OSErr read_header( rsrc_fork_header& header )
{
	OSErr err;
	Size size = sizeof header;
	
	(err = SetFPos( refnum, fsFromStart, 0 ))  ||
	(err = FSRead ( refnum, &size, &header ));
	
	return err;
}

static
void init()
{
	using mac::sys::res_error;
	
	const OSErr noErr = 0;
	
	HDelete( vRefNum, dirID, name );
	
	HCreateResFile( vRefNum, dirID, name );
	
	EXPECT_EQ( res_error(), noErr );
	
	refnum = HOpenResFile( vRefNum, dirID, name, fsRdWrPerm );
	
	EXPECT_EQ( res_error(), noErr );
}

static
void one_rsrc()
{
	const OSErr noErr = 0;
	
	const short resChanged = 2;
	
	const short mapChanged = 32;
	const short mapCompact = 64;
	
	rsrc_fork_header header;
	
	EXPECT_EQ( GetResFileAttrs( refnum ), 0 );
	
	EXPECT_EQ( geteof(), 286 );
	
	EXPECT_EQ( read_header( header ), noErr );
	
	EXPECT_EQ( header.offset_to_data, 256 );
	EXPECT_EQ( header.offset_to_map,  256 );
	EXPECT_EQ( header.length_of_data,   0 );
	EXPECT_EQ( header.length_of_map,   30 );
	
	EXPECT_ZERO_DATA( 128, 128 );  // file offset = 128, length = 128
	
	EXPECT_CMP_DATA( 256, 30, EMPTY_RSRC_MAP, 30 );
	
	if ( Handle h = NewHandle( 11 ) )
	{
		BlockMoveData( "Hello world", *h, 11 );
		
		AddResource( h, 'TEXT', 128, NULL );
		
		EXPECT_EQ( GetResAttrs( h ), resChanged );
		
		EXPECT_EQ( GetResFileAttrs( refnum ), mapChanged | mapCompact );
		
		EXPECT_EQ( geteof(), 321 );
		
		read_header( header );
		
		EXPECT_EQ( header.offset_to_data, 256 );
		EXPECT_EQ( header.offset_to_map,  256 );
		EXPECT_EQ( header.length_of_data,   0 );
		EXPECT_EQ( header.length_of_map,   30 );
		
		EXPECT_ZERO_DATA( 286, 35 );
		
		WriteResource( h );
		
		EXPECT_EQ( GetResAttrs( h ), 0 );
		
		EXPECT_EQ( GetResFileAttrs( refnum ), mapChanged | mapCompact );
		
		EXPECT_EQ( geteof(), 321 );
		
		read_header( header );
		
		EXPECT_EQ( header.offset_to_data, 256 );
		EXPECT_EQ( header.offset_to_map,  256 );
		EXPECT_EQ( header.length_of_data,   0 );
		EXPECT_EQ( header.length_of_map,   30 );
		
		EXPECT_CMP_DATA( 256, 30, EMPTY_RSRC_MAP, 30 );
		EXPECT_ZERO_DATA( 286, 20 );
		EXPECT_CMP_DATA( 306, 15, "\0\0\0\x0b" "Hello world", 15 );
		
		UpdateResFile( refnum );
		
		EXPECT_EQ( GetResFileAttrs( refnum ), 0 );
		
		EXPECT_EQ( geteof(), 321 );
		
		read_header( header );
		
		EXPECT_EQ( header.offset_to_data, 256 );
		EXPECT_EQ( header.offset_to_map,  271 );
		EXPECT_EQ( header.length_of_data,  15 );
		EXPECT_EQ( header.length_of_map,   50 );
		
		EXPECT_CMP_DATA( 256, 15, "\0\0\0\x0b" "Hello world!", 15 );
		EXPECT_CMP_DATA( 271, 16, "\0\0\1\0" "\0\0\1\x0f" "\0\0\0\x0f" "\0\0\0\x32", 16 );
		EXPECT_CMP_DATA( 293,  6, "\0\0" "\0\x1c" "\0\x32", 6 );
		EXPECT_CMP_DATA( 299, 10, "\0\0" "TEXT" "\0\0\0\x0a", 10 );
		EXPECT_CMP_DATA( 309,  8, "\0\x80" "\xff\xff" ZERO_x4, 8 );
		
		SetHandleSize( h, 12 );
		
		h[0][ 11 ] = '!';
		
		// Hello world!
		
		ChangedResource( h );
		
		EXPECT_EQ( GetResAttrs( h ), resChanged );
		
		EXPECT_EQ( GetResFileAttrs( refnum ), mapChanged | mapCompact );
		
		EXPECT_EQ( geteof(), 337 );  // got 322
		
		read_header( header );
		
		EXPECT_EQ( header.offset_to_data, 256 );
		EXPECT_EQ( header.offset_to_map,  271 );
		EXPECT_EQ( header.length_of_data,  15 );
		EXPECT_EQ( header.length_of_map,   50 );
		
		EXPECT_ZERO_DATA( 321, 16 );
		
		WriteResource( h );
		
		EXPECT_EQ( GetResAttrs( h ), 0 );
		
		EXPECT_EQ( GetResFileAttrs( refnum ), mapChanged | mapCompact );
		
		EXPECT_EQ( geteof(), 337 );
		
		read_header( header );
		
		EXPECT_EQ( header.offset_to_data, 256 );
		EXPECT_EQ( header.offset_to_map,  271 );
		EXPECT_EQ( header.length_of_data,  15 );
		EXPECT_EQ( header.length_of_map,   50 );
		
		EXPECT_CMP_DATA( 321, 16, "\0\0\0\x0c" "Hello world!", 16 );
		
		UpdateResFile( refnum );
		
		EXPECT_EQ( GetResFileAttrs( refnum ), 0 );
		
		EXPECT_EQ( geteof(), 322 );
		
		read_header( header );
		
		EXPECT_EQ( header.offset_to_data, 256 );
		EXPECT_EQ( header.offset_to_map,  272 );
		EXPECT_EQ( header.length_of_data,  16 );
		EXPECT_EQ( header.length_of_map,   50 );
		
		EXPECT_CMP_DATA( 256, 16, "\0\0\0\x0c" "Hello world!", 16 );
		EXPECT_CMP_DATA( 272, 16, "\0\0\1\0" "\0\0\1\x10" "\0\0\0\x10" "\0\0\0\x32", 16 );
		EXPECT_CMP_DATA( 294,  6, "\0\0" "\0\x1c" "\0\x32", 6 );
		EXPECT_CMP_DATA( 300, 10, "\0\0" "TEXT" "\0\0\0\x0a", 10 );
		EXPECT_CMP_DATA( 310,  8, "\0\x80" "\xff\xff" ZERO_x4, 8 );
		
		SetHandleSize( h, 11 );
		
		// Hello world
		
		ChangedResource( h );
		
		EXPECT_EQ( GetResAttrs( h ), resChanged );
		
		EXPECT_EQ( GetResFileAttrs( refnum ), mapChanged );
		
		EXPECT_EQ( geteof(), 322 );
		
		read_header( header );
		
		EXPECT_EQ( header.offset_to_data, 256 );
		EXPECT_EQ( header.offset_to_map,  272 );
		EXPECT_EQ( header.length_of_data,  16 );
		EXPECT_EQ( header.length_of_map,   50 );
		
		WriteResource( h );
		
		EXPECT_EQ( GetResAttrs( h ), 0 );
		
		EXPECT_EQ( GetResFileAttrs( refnum ), mapChanged );
		
		EXPECT_EQ( geteof(), 322 );
		
		read_header( header );
		
		EXPECT_EQ( header.offset_to_data, 256 );
		EXPECT_EQ( header.offset_to_map,  272 );
		EXPECT_EQ( header.length_of_data,  16 );
		EXPECT_EQ( header.length_of_map,   50 );
		
		EXPECT_CMP_DATA( 256, 16, "\0\0\0\x0b" "Hello world!", 16 );
		
		UpdateResFile( refnum );
		
		EXPECT_EQ( GetResFileAttrs( refnum ), 0 );
		
		EXPECT_EQ( geteof(), 322 );
		
		read_header( header );
		
		EXPECT_EQ( header.offset_to_data, 256 );
		EXPECT_EQ( header.offset_to_map,  272 );
		EXPECT_EQ( header.length_of_data,  16 );
		EXPECT_EQ( header.length_of_map,   50 );
		
		SetResInfo( h, 128, NULL );
		
		EXPECT_EQ( GetResFileAttrs( refnum ), mapChanged );
		
		SetHandleSize( h, 12 );
		
		h[0][ 11 ] = '?';
		
		// Hello world?
		
		ChangedResource( h );
		
		EXPECT_EQ( GetResAttrs( h ), resChanged );
		
		EXPECT_EQ( GetResFileAttrs( refnum ), mapChanged );
		
		EXPECT_EQ( geteof(), 322 );
		
		read_header( header );
		
		EXPECT_EQ( header.offset_to_data, 256 );
		EXPECT_EQ( header.offset_to_map,  272 );
		EXPECT_EQ( header.length_of_data,  16 );
		EXPECT_EQ( header.length_of_map,   50 );
		
		WriteResource( h );
		
		EXPECT_EQ( geteof(), 322 );
		
		EXPECT_CMP_DATA( 256, 16, "\0\0\0\x0c" "Hello world?", 16 );
		
		SetResInfo( h, 128, "\p" "Hello?" );
		
		EXPECT_EQ( GetResAttrs( h ), 0 );
		
		EXPECT_EQ( GetResFileAttrs( refnum ), mapChanged );
		
		EXPECT_EQ( geteof(), 329 );
		
		EXPECT_ZERO_DATA( 322, 7 );
		
		ChangedResource( h );
		
		EXPECT_EQ( GetResAttrs( h ), resChanged );
		
		EXPECT_EQ( GetResFileAttrs( refnum ), mapChanged );
		
		EXPECT_EQ( geteof(), 329 );
		
		EXPECT_ZERO_DATA( 322, 7 );
		
		WriteResource( h );
		
		EXPECT_EQ( GetResAttrs( h ), 0 );
		
		EXPECT_EQ( GetResFileAttrs( refnum ), mapChanged );
		
		EXPECT_ZERO_DATA( 322, 7 );
		
		UpdateResFile( refnum );
		
		EXPECT_CMP_DATA( 322, 7, "\p" "Hello?", 7 );
		
		SetResInfo( h, 128, "\p" "cello" );
		
		EXPECT_EQ( GetResAttrs( h ), 0 );
		
		EXPECT_EQ( GetResFileAttrs( refnum ), mapChanged );
		
		EXPECT_EQ( geteof(), 328 );
		
		EXPECT_CMP_DATA( 322, 6, "\p" "Hello?", 6 );  // !!
		
		UpdateResFile( refnum );
		
		EXPECT_CMP_DATA( 322, 6, "\p" "cello", 6 );
		
		RemoveResource( h );
		
		EXPECT_EQ( HGetState( h ), 0 );
		
		DisposeHandle( h );
		
		EXPECT_EQ( GetResFileAttrs( refnum ), mapChanged | mapCompact );
		
		EXPECT_EQ( geteof(), 328 );
		
		EXPECT_CMP_DATA( 256, 16, "\0\0\0\x0c" "Hello world?", 16 );
		
		UpdateResFile( refnum );
		
		EXPECT_EQ( geteof(), 286 );
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
	
	one_rsrc();
	
	term();
	
	return 0;
}

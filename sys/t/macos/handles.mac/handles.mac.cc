/*
	handles.mac.cc
	--------------
*/

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __MACERRORS__
#include <MacErrors.h>
#endif
#ifndef __MACMEMORY__
#include <MacMemory.h>
#endif

// Standard C
#include <stdint.h>
#include <string.h>

// mac-sys-utils
#include "mac_sys/gestalt.hh"

// tap-out
#include "tap/test.hh"


#pragma exceptions off


static const unsigned n_tests = 2 + 5 + 5 * 4 + 5 + 13 + 14;


#define STRLEN( s )  (sizeof "" s - 1)

#define STR_LEN( s )  "" s, STRLEN( s )


using tap::ok_if;


static bool in_v68k()
{
	static bool b = mac::sys::gestalt_defined( 'v68k' );
	
	return b;
}

static uint32_t get_sysv()
{
	static uint32_t sysv = mac::sys::gestalt( 'sysv' );
	
	return sysv;
}

static inline bool in_os9()
{
	return !in_v68k()  &&  get_sysv() < 0x1000;
}

static inline bool in_osx()
{
	return !in_v68k()  &&  get_sysv() >= 0x1000;
}

static void new_empty()
{
	Handle a = NewEmptyHandle();
	
	ok_if( a != NULL );
	ok_if( *a == NULL );
	
	DisposeHandle( a );
}

static void handle_0()
{
	Handle a = NewHandle( 0 );
	
	ok_if( a != NULL );
	ok_if( *a != NULL );
	
	ok_if( !in_v68k()  ||  ((uint32_t*) *a)[ -1 ] == 0xC7C7C7C7 );
	ok_if( !in_v68k()  ||  ((uint32_t*) *a)[  0 ] == 0xC8C8C8C8 );
	
	ok_if( GetHandleSize( a ) == 0 );
	
	DisposeHandle( a );
}

static void handle_1234( int n )
{
	Handle a = NewHandle( n );
	
	ok_if( a != NULL );
	ok_if( *a != NULL );
	
	ok_if( !in_v68k()  ||  ((uint32_t*) *a)[ -1 ] == 0xC7C7C7C7 );
	ok_if( !in_v68k()  ||  ((uint32_t*) *a)[  1 ] == 0xC8C8C8C8 );
	
	ok_if( GetHandleSize( a ) == n );
	
	DisposeHandle( a );
}

static void handle_5()
{
	Handle a = NewHandle( 5 );
	
	ok_if( a != NULL );
	ok_if( *a != NULL );
	
	ok_if( !in_v68k()  ||  ((uint32_t*) *a)[ -1 ] == 0xC7C7C7C7 );
	ok_if( !in_v68k()  ||  ((uint32_t*) *a)[  2 ] == 0xC8C8C8C8 );
	
	ok_if( GetHandleSize( a ) == 5 );
	
	DisposeHandle( a );
}

static bool is_clear( const char* begin, const char* end )
{
	while ( begin < end )
	{
		if ( *begin++ != '\0' )
		{
			return false;
		}
	}
	
	return true;
}

static void resize()
{
	int size = STRLEN( "This is a test." );  // 15
	
	Handle a = NewHandleClear( size );
	
	ok_if( a != NULL );
	ok_if( *a != NULL );
	
	ok_if( GetHandleSize( a ) == size );
	
	ok_if( is_clear( *a, *a + size ) );
	
	memcpy( *a, STR_LEN( "This is a test." ) );
	
	size = STRLEN( "This is a te" );  // 12
	
	SetHandleSize( a, size );
	
	ok_if( GetHandleSize( a ) == size );
	
	ok_if( memcmp( *a, STR_LEN( "This is a te" ) ) == 0 );
	
	ok_if( !in_v68k()  ||  ((uint32_t*) *a)[ -1 ] == 0xC7C7C7C7 );
	ok_if( !in_v68k()  ||  ((uint32_t*) *a)[  3 ] == 0xC8C8C8C8 );
	
	size = 100;
	
	SetHandleSize( a, size );
	
	ok_if( GetHandleSize( a ) == size );
	
	ok_if( memcmp( *a, STR_LEN( "This is a te" ) ) == 0 );
	
	ok_if( !in_v68k()  ||  ((uint32_t*) *a)[ -1 ] == 0xC7C7C7C7 );
	ok_if( !in_v68k()  ||  ((uint32_t*) *a)[ 25 ] == 0xC8C8C8C8 );
	
	EmptyHandle( a );
	
	ok_if( *a == NULL );
	
	DisposeHandle( a );
}

static void errors()
{
	const uint32_t sysv = get_sysv();
	
	Handle neg1 = NewHandle( -1 );
	
	ok_if( (neg1 == NULL) == (in_v68k()  ||  sysv >= 0x0800) );
	
	if ( neg1 != NULL )
	{
		ok_if( GetHandleSize( neg1 ) == 0 );
		
		DisposeHandle( neg1 );
	}
	else
	{
		ok_if( MemError() == memSCErr );
	}
	
	ok_if( GetHandleSize( NULL ) == 0 );
	
	DisposeHandle( NULL );
	
	ok_if( MemError() == (in_os9() ? 0 : nilHandleErr) );
	
	ReallocateHandle( NULL, 1 );
	
	ok_if( MemError() == (in_os9() ? 0 : nilHandleErr) );
	
	ReallocateHandle( NULL, -1 );
	
	ok_if( MemError() == (in_os9() ? 0 : nilHandleErr) );
	
	Handle a = NewEmptyHandle();
	
	ok_if( GetHandleSize( a ) == 0 );
	
	SetHandleSize( a, 1 );
	
	ok_if( MemError() == nilHandleErr );
	
	SetHandleSize( NULL, 1 );
	
	ok_if( MemError() == nilHandleErr );
	
	SetHandleSize( NULL, -1 );
	
	ok_if( MemError() == nilHandleErr );
	
	Handle b = NewHandle( 1 );
	
	SetHandleSize( b, -1 );
	
	ok_if( MemError() == (in_os9() ? 0 : memSCErr) );
	
	ok_if( in_os9() ? GetHandleSize( b ) == 0 : true );
	
	if ( !in_osx() )
	{
		SetHandleSize( b, 1024 * 1024 * 1024 );  // 1 GiB
		
		ok_if( MemError() == memFullErr );
		
		Handle c = NewHandle( 1024 * 1024 * 1024 );  // 1 GiB
		
		ok_if( MemError() == memFullErr );
	}
	else
	{
		ok_if( true );
		ok_if( true );
	}
}


int main( int argc, char** argv )
{
	tap::start( "handles.mac", n_tests );
	
	new_empty();
	
	handle_0();
	
	handle_1234( 1 );
	handle_1234( 2 );
	handle_1234( 3 );
	handle_1234( 4 );
	
	handle_5();
	
	resize();
	
	errors();
	
	return 0;
}

/*
	AESizeOfFlattenedDesc.cc
	------------------------
*/

// Standard C
#include <stdlib.h>

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __AEDATAMODEL__
#include <AEDataModel.h>
#endif

#undef check

// iota
#include "iota/strings.hh"

// mac-sys-utils
#include "mac_sys/gestalt.hh"

// tap-out
#include "tap/check.hh"
#include "tap/test.hh"


static const unsigned n_tests = 1
                              + 1
                              + 1
                              + 2
                              + 1;


using mac::sys::gestalt;

const uint32_t sysv = TARGET_API_MAC_CARBON ? gestalt( 'sysv' ) : 0;

const bool panther = TARGET_API_MAC_CARBON  &&  sysv >= 0x1030;

static
void null_NULL()
{
	const AEDesc null = { typeNull, NULL };
	
	Size size = AESizeOfFlattenedDesc( &null );
	
	EXPECT_EQ( size, 16 );
}

static
void word_NULL()
{
	const AEDesc word_null = { 'word', NULL };
	
	Size size = AESizeOfFlattenedDesc( &word_null );
	
	EXPECT_EQ( size, 16 );
}

static
void true_NULL()
{
	const AEDesc true_null = { typeTrue, NULL };
	
	Size size = AESizeOfFlattenedDesc( &true_null );
	
	EXPECT_EQ( size, 16 );
}

static
void create_typeNull_with_data()
{
	AEDesc null_h = { typeNull, NULL };
	
	if ( AECreateDesc( typeNull, STR_LEN( "Josh" ), &null_h ) != noErr )
	{
		exit( 1 );
	}
	
	EXPECT_NULL( null_h.dataHandle );
	
	Size size = AESizeOfFlattenedDesc( &null_h );
	
	EXPECT_EQ( size, 16 );
}

static
void create_with_data_and_set_typeNull()
{
	using mac::sys::gestalt;
	
	AEDesc null_h = { typeNull, NULL };
	
	if ( AECreateDesc( typeType, STR_LEN( "Josh" ), &null_h ) != noErr )
	{
		exit( 1 );
	}
	
	null_h.descriptorType = typeNull;
	
	Size size = AESizeOfFlattenedDesc( &null_h );
	
	const Size expected_size = panther ? 20 : 16;
	
	EXPECT_EQ( size, expected_size );
	
	AEDisposeDesc( &null_h );
}

int main( int argc, char** argv )
{
	tap::start( "AESizeOfFlattenedDesc", n_tests );
	
	null_NULL();
	word_NULL();
	true_NULL();
	
	create_typeNull_with_data();
	
	create_with_data_and_set_typeNull();
	
	return 0;
}

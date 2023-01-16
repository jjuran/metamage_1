/*
	AEGetDescData.cc
	----------------
*/

// Standard C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Mac OS
#include <AEDataModel.h>
#include <MacErrors.h>
#include <MacMemory.h>

#undef check

// iota
#include "iota/strings.hh"

// tap-out
#include "tap/check.hh"
#include "tap/test.hh"


static const unsigned n_tests = 2
                              + 2
                              + 2
                              + 3
                              + 2
                              + !TARGET_API_MAC_CARBON * 2;


enum Carbon_test_mode
{
	test_default,
	test_OS_9,  // CarbonLib
	test_OS_X   // Carbon.framework
};

Carbon_test_mode global_test_mode = test_default;

static bool passed_for_mode( bool value_for_OS_9, bool value_for_OS_X )
{
	const bool consider_OS_9 = !TARGET_RT_MAC_MACHO    && !global_test_mode  ||  global_test_mode == test_OS_9;
	const bool consider_OS_X =  TARGET_API_MAC_CARBON  && !global_test_mode  ||  global_test_mode == test_OS_X;
	
	return     consider_OS_9  &&  value_for_OS_9
	       ||  consider_OS_X  &&  value_for_OS_X;
}

static void null_NULL()
{
	char buffer[] = "1234567890abcdef";
	
	const AEDesc null = { typeNull, NULL };
	
	OSErr err = AEGetDescData( &null, buffer, sizeof buffer - 1 );
	
	const bool ok_for_OS_9 = err == noErr;
	const bool ok_for_OS_X = err == paramErr;
	
	EXPECT( passed_for_mode( ok_for_OS_9, ok_for_OS_X ) );
	
	EXPECT( memcmp( buffer, STR_LEN( "1234567890abcdef" ) ) == 0 );
}

static void word_NULL()
{
	char buffer[] = "1234567890abcdef";
	
	const DescType word = TARGET_RT_BIG_ENDIAN ? '!@#$' : '$#@!';
	
	const AEDesc word_null = { word, NULL };
	
	OSErr err = AEGetDescData( &word_null, buffer, sizeof buffer - 1 );
	
	EXPECT( err == noErr );
	
	const bool unchanged = memcmp( buffer, STR_LEN( "1234567890abcdef" ) ) == 0;
	const bool got_type  = memcmp( buffer, STR_LEN( "!@#$567890abcdef" ) ) == 0;
	
	EXPECT( passed_for_mode( unchanged, got_type ) );
}

#if TARGET_RT_BIG_ENDIAN
#define TRUE_STRING "true"
#else
#define TRUE_STRING "eurt"
#endif

static void true_NULL()
{
	char buffer[] = "1234567890abcdef";
	
	const AEDesc true_null = { typeTrue, NULL };
	
	OSErr err = AEGetDescData( &true_null, buffer, sizeof buffer - 1 );
	
	EXPECT( err == noErr );
	
	const bool unchanged = memcmp( buffer, STR_LEN( "1234567890abcdef" ) ) == 0;
	const bool got_type  = memcmp( buffer, STR_LEN( TRUE_STRING "567890abcdef" ) ) == 0;
	
	EXPECT( passed_for_mode( unchanged, got_type ) );
}

static void create_typeNull_with_data()
{
	char buffer[] = "1234567890abcdef";
	
	AEDesc null_h = { typeNull, NULL };
	
	if ( AECreateDesc( typeNull, STR_LEN( "Josh" ), &null_h ) != noErr )
	{
		exit( 1 );
	}
	
	EXPECT( null_h.dataHandle == NULL );
	
	OSErr err = AEGetDescData( &null_h, buffer, sizeof buffer - 1 );
	
	const bool ok_for_OS_9 = err == noErr;
	const bool ok_for_OS_X = err == paramErr;
	
	EXPECT( passed_for_mode( ok_for_OS_9, ok_for_OS_X ) );
	
	const bool unchanged = memcmp( buffer, STR_LEN( "1234567890abcdef" ) ) == 0;
	
	EXPECT( unchanged );
}

static void create_with_data_and_set_typeNull()
{
	char buffer[] = "1234567890abcdef";
	
	AEDesc null_h = { typeNull, NULL };
	
	if ( AECreateDesc( typeType, STR_LEN( "Josh" ), &null_h ) != noErr )
	{
		exit( 1 );
	}
	
	null_h.descriptorType = typeNull;
	
	OSErr err = AEGetDescData( &null_h, buffer, sizeof buffer - 1 );
	
	const bool ok_for_OS_9 = err == noErr;
	const bool ok_for_OS_X = err == paramErr;
	
	EXPECT( passed_for_mode( ok_for_OS_9, ok_for_OS_X ) );
	
	const bool got_data  = memcmp( buffer, STR_LEN( "Josh567890abcdef" ) ) == 0;
	const bool unchanged = memcmp( buffer, STR_LEN( "1234567890abcdef" ) ) == 0;
	
	EXPECT( passed_for_mode( got_data, unchanged ) );
	
	AEDisposeDesc( &null_h );
}

static void purged_handle()
{
#if !TARGET_API_MAC_CARBON
	
	char buffer[] = "1234567890abcdef";
	
	AEDesc desc = { typeNull, NULL };
	
	if ( AECreateDesc( typeType, STR_LEN( "Josh" ), &desc ) != noErr )
	{
		exit( 1 );
	}
	
	EmptyHandle( desc.dataHandle );
	
	OSErr err = AEGetDescData( &desc, buffer, sizeof buffer - 1 );
	
	EXPECT( err == noErr );
	
	const bool unchanged = memcmp( buffer, STR_LEN( "1234567890abcdef" ) ) == 0;
	
	EXPECT( unchanged );
	
	AEDisposeDesc( &desc );
	
#endif
}

int main( int argc, char** argv )
{
	if ( argc > 1 )
	{
		if ( strcmp( argv[ 1 ], "-9" ) == 0 )
		{
			global_test_mode = test_OS_9;
		}
		else if ( strcmp( argv[ 1 ], "-X" ) == 0 )
		{
			global_test_mode = test_OS_X;
		}
	}
	
	tap::start( "AEGetDescData", n_tests );
	
	null_NULL();
	
	word_NULL();
	
	true_NULL();
	
	create_typeNull_with_data();
	
	create_with_data_and_set_typeNull();
	
	purged_handle();
	
	return 0;
}

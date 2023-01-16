/*
	AEGetDescDataSize.cc
	--------------------
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


static const unsigned n_tests = 1
                              + 1
                              + 1
                              + 2
                              + 1
                              + !TARGET_API_MAC_CARBON;


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
	const AEDesc null = { typeNull, NULL };
	
	Size size = AEGetDescDataSize( &null );
	
	EXPECT( size == 0 );
}

static void word_NULL()
{
	const AEDesc word_null = { 'word', NULL };
	
	Size size = AEGetDescDataSize( &word_null );
	
	const bool ok_for_OS_9 = size == 0;
	const bool ok_for_OS_X = size == 4;
	
	EXPECT( passed_for_mode( ok_for_OS_9, ok_for_OS_X ) );
}

static void true_NULL()
{
	const AEDesc true_null = { typeTrue, NULL };
	
	Size size = AEGetDescDataSize( &true_null );
	
	const bool ok_for_OS_9 = size == 0;
	const bool ok_for_OS_X = size == 4;
	
	EXPECT( passed_for_mode( ok_for_OS_9, ok_for_OS_X ) );
}

static void create_typeNull_with_data()
{
	AEDesc null_h = { typeNull, NULL };
	
	if ( AECreateDesc( typeNull, STR_LEN( "Josh" ), &null_h ) != noErr )
	{
		exit( 1 );
	}
	
	EXPECT( null_h.dataHandle == NULL );
	
	Size size = AEGetDescDataSize( &null_h );
	
	EXPECT( size == 0 );
}

static void create_with_data_and_set_typeNull()
{
	AEDesc null_h = { typeNull, NULL };
	
	if ( AECreateDesc( typeType, STR_LEN( "Josh" ), &null_h ) != noErr )
	{
		exit( 1 );
	}
	
	null_h.descriptorType = typeNull;
	
	Size size = AEGetDescDataSize( &null_h );
	
	EXPECT( size == 0 );
	
	AEDisposeDesc( &null_h );
}

static void purged_handle()
{
#if !TARGET_API_MAC_CARBON
	
	AEDesc desc = { typeNull, NULL };
	
	if ( AECreateDesc( typeType, STR_LEN( "Josh" ), &desc ) != noErr )
	{
		exit( 1 );
	}
	
	EmptyHandle( desc.dataHandle );
	
	Size size = AEGetDescDataSize( &desc );
	
	EXPECT( size == 0 );
	
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
	
	tap::start( "AEGetDescDataSize", n_tests );
	
	null_NULL();
	
	word_NULL();
	
	true_NULL();
	
	create_typeNull_with_data();
	
	create_with_data_and_set_typeNull();
	
	purged_handle();
	
	return 0;
}

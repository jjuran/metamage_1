/*
	v68k-macros.cc
	--------------
*/

// Standard C
#include <string.h>

// v68k
#include "v68k/macros.hh"

// tap-out
#include "tap/test.hh"


#pragma exceptions off


static const unsigned n_tests = 9 + 3;


static void sign_extend()
{
	using namespace v68k;
	
	EXPECT( sign_extend( 0, byte_sized ) == 0 );
	EXPECT( sign_extend( 0, word_sized ) == 0 );
	EXPECT( sign_extend( 0, long_sized ) == 0 );
	
	EXPECT( sign_extend( 0x12345678, byte_sized ) == 0x00000078 );
	EXPECT( sign_extend( 0x12345678, word_sized ) == 0x00005678 );
	EXPECT( sign_extend( 0x12345678, long_sized ) == 0x12345678 );
	
	EXPECT( sign_extend( 0xFEDCBA98, byte_sized ) == 0xFFFFFF98 );
	EXPECT( sign_extend( 0xFEDCBA98, word_sized ) == 0xFFFFBA98 );
	EXPECT( sign_extend( 0xFEDCBA98, long_sized ) == 0xFEDCBA98 );
}

static void update()
{
	using namespace v68k;
	
	EXPECT( update( 0x12345678, 0x01020304, byte_sized ) == 0x12345604 );
	EXPECT( update( 0x12345678, 0x01020304, word_sized ) == 0x12340304 );
	EXPECT( update( 0x12345678, 0x01020304, long_sized ) == 0x01020304 );
}

int main( int argc, char** argv )
{
	tap::start( "v68k-macros", n_tests );
	
	sign_extend();
	
	update();
	
	return 0;
}

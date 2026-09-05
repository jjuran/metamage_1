/*
	fractions.cc
	------------
*/

// math
#include "math/div_65536.hh"
#include "math/fractions.hh"

// tap-out
#include "tap/check.hh"
#include "tap/test.hh"


static const unsigned n_tests = 11 * 3 + 13 + 1;

template < class Int >
void test_half( Int x )
{
	EXPECT_EQ( math::fractions::half( x ), x / 2 );
}

static
void byte()
{
	test_half( (signed char)  3 );
	test_half( (signed char)  2 );
	test_half( (signed char)  1 );
	test_half( (signed char)  0 );
	test_half( (signed char) -1 );
	test_half( (signed char) -2 );
	test_half( (signed char) -3 );
	
	test_half( (signed char)  126 );
	test_half( (signed char)  127 );
	test_half( (signed char) -128 );
	test_half( (signed char) -127 );
}

static
void word()
{
	test_half( (short)  3 );
	test_half( (short)  2 );
	test_half( (short)  1 );
	test_half( (short)  0 );
	test_half( (short) -1 );
	test_half( (short) -2 );
	test_half( (short) -3 );
	
	test_half( (short)  32766 );
	test_half( (short)  32767 );
	test_half( (short) -32768 );
	test_half( (short) -32767 );
}

static
void longword()
{
	test_half( (long)  3 );
	test_half( (long)  2 );
	test_half( (long)  1 );
	test_half( (long)  0 );
	test_half( (long) -1 );
	test_half( (long) -2 );
	test_half( (long) -3 );
	
	test_half( (long)  2147483646  );
	test_half( (long)  2147483647  );
	test_half( (long) -2147483648u );
	test_half( (long) -2147483647  );
}

#define EXPECT_EQ_DIV_65536( x )  EXPECT_EQ( div_65536( x ), x / 65536 )

static
void div_65536()
{
	using math::fractions::div_65536;
	
	EXPECT_EQ_DIV_65536( (short)  32767 );
	EXPECT_EQ_DIV_65536( (short) -32768 );
	
	EXPECT_EQ_DIV_65536( (unsigned short) 65535 );
	
	EXPECT_EQ_DIV_65536( (long) 0 );
	EXPECT_EQ_DIV_65536( (long) 1 );
	EXPECT_EQ_DIV_65536( (long) 65535 );
	EXPECT_EQ_DIV_65536( (long) 65536 );
	EXPECT_EQ_DIV_65536( (long) -1 );
	EXPECT_EQ_DIV_65536( (long) -65535 );
	EXPECT_EQ_DIV_65536( (long) -65536 );
	
	EXPECT_EQ_DIV_65536( (long) 0x7FFFFFFF );
	EXPECT_EQ_DIV_65536( (long) 0x80000000 );
	EXPECT_EQ_DIV_65536( (long) 0x80000001 );
}

static
void div_65536_ULL()
{
	unsigned long long a = 0xfedcba9876543210;
	unsigned long long b = a / 65536;
	
	math::fractions::div_65536_ULL( &a );
	
	EXPECT_EQ( a, b );
}

int main( int argc, char** argv )
{
	tap::start( "fractions", n_tests );
	
	byte();
	word();
	longword();
	
	div_65536();
	div_65536_ULL();
	
	return 0;
}

/*
	t/decimal.cc
	------------
*/

// iota
#include "iota/strings.hh"

// gear
#include "gear/inscribe_decimal.hh"

// tap-out
#include "tap/test.hh"


#define PROGRAM  "decimal"

static const unsigned n_tests = 30 + 4 + 6 + 7;


static void magnitude()
{
	using gear::pure_magnitude;
	using gear::magnitude;
	
	EXPECT( pure_magnitude< 10 >( 0 ) == 0 );
	EXPECT( pure_magnitude< 10 >( 1 ) == 1 );
	EXPECT( pure_magnitude< 10 >( 2 ) == 1 );
	EXPECT( pure_magnitude< 10 >( 9 ) == 1 );
	EXPECT( pure_magnitude< 10 >( 10 ) == 2 );
	EXPECT( pure_magnitude< 10 >( 99 ) == 2 );
	EXPECT( pure_magnitude< 10 >( 100 ) == 3 );
	EXPECT( pure_magnitude< 10 >( 999 ) == 3 );
	EXPECT( pure_magnitude< 10 >( 1000 ) == 4 );
	EXPECT( pure_magnitude< 10 >( 9999 ) == 4 );
	EXPECT( pure_magnitude< 10 >( 10000 ) == 5 );
	EXPECT( pure_magnitude< 10 >( 99999 ) == 5 );
	EXPECT( pure_magnitude< 10 >( 100000 ) == 6 );
	EXPECT( pure_magnitude< 10 >( 999999 ) == 6 );
	EXPECT( pure_magnitude< 10 >( 1000000 ) == 7 );
	EXPECT( pure_magnitude< 10 >( 9999999 ) == 7 );
	EXPECT( pure_magnitude< 10 >( 10000000 ) == 8 );
	EXPECT( pure_magnitude< 10 >( 99999999 ) == 8 );
	EXPECT( pure_magnitude< 10 >( 100000000 ) == 9 );
	EXPECT( pure_magnitude< 10 >( 999999999 ) == 9 );
	EXPECT( pure_magnitude< 10 >( 1000000000 ) == 10 );
	EXPECT( pure_magnitude< 10 >( 2147483647 ) == 10 );
	EXPECT( pure_magnitude< 10 >( 2147483648 ) == 10 );
	EXPECT( pure_magnitude< 10 >( 4294967295 ) == 10 );
	EXPECT( pure_magnitude< 10 >( 4294967296 ) == 10 );
	EXPECT( pure_magnitude< 10 >( 9223372036854775807 ) == 19 );
	EXPECT( pure_magnitude< 10 >( 9223372036854775808u ) == 19 );
	EXPECT( pure_magnitude< 10 >( 18446744073709551615u ) == 20 );
	
	EXPECT( magnitude< 10 >( 0 ) == 1 );
	EXPECT( magnitude< 10 >( 1 ) == 1 );
}

#define Xx24 "xxxxxxxx" "xxxxxxxx" "xxxxxxxx"

static void fill()
{
	using gear::fill_unsigned;
	
	char buffer[] = Xx24;
	
	fill_unsigned< 10 >( 0, buffer, buffer );
	
	EXPECT_CMP( buffer, sizeof buffer, Xx24, sizeof Xx24 );
	
	fill_unsigned< 10 >( 0, buffer + 3, buffer + 7 );
	
	EXPECT_CMP( buffer, 8, "xxx0000x", 8 );
	
	fill_unsigned< 10 >( 12345, buffer + 2, buffer + 6 );
	
	EXPECT_CMP( buffer, 8, "xx23450x", 8 );
	
	fill_unsigned< 10 >( 12345, buffer + 1, buffer + 7 );
	
	EXPECT_CMP( buffer, 8, "x012345x", 8 );
}

static void inscribe_unsigned()
{
	using gear::inscribe_unsigned_r;
	
	char buffer[] = Xx24;
	
	char* end;
	
	end = inscribe_unsigned_r< 10 >( 0, buffer );
	
	EXPECT_CMP( buffer, 8, "0xxxxxxx", 8 );
	
	end = inscribe_unsigned_r< 10 >( 12345, buffer );
	
	EXPECT_CMP( buffer, 8, "12345xxx", 8 );
	
	end = inscribe_unsigned_r< 10 >( 2147483648, buffer );
	
	EXPECT_CMP( buffer, 12, "2147483648xx", 12 );
	
	end = inscribe_unsigned_r< 10 >( 4294967296, buffer );
	
	EXPECT_CMP( buffer, 12, "4294967296xx", 12 );
	
	end = inscribe_unsigned_r< 10 >( 9223372036854775808u, buffer );
	
	EXPECT_CMP( buffer, 24, "9223372036854775808xxxxx", 24 );
	
	end = inscribe_unsigned_r< 10 >( 18446744073709551615u, buffer );
	
	EXPECT_CMP( buffer, 24, "18446744073709551615xxxx", 24 );
}

static void inscribe_signed()
{
	using gear::inscribe_signed_r;
	
	char buffer[] = Xx24;
	
	char* end;
	
	end = inscribe_signed_r< 10 >( 0, buffer );
	
	EXPECT_CMP( buffer, 8, "0xxxxxxx", 8 );
	
	end = inscribe_signed_r< 10 >( 12345, buffer );
	
	EXPECT_CMP( buffer, 8, "12345xxx", 8 );
	
	end = inscribe_signed_r< 10 >( 2147483648, buffer );
	
	EXPECT_CMP( buffer, 12, "2147483648xx", 12 );
	
	end = inscribe_signed_r< 10 >( 4294967296, buffer );
	
	EXPECT_CMP( buffer, 12, "4294967296xx", 12 );
	
	end = inscribe_signed_r< 10 >( 9223372036854775807, buffer );
	
	EXPECT_CMP( buffer, 24, "9223372036854775807xxxxx", 24 );
	
	end = inscribe_signed_r< 10 >( -9223372036854775807, buffer );
	
	EXPECT_CMP( buffer, 24, "-9223372036854775807xxxx", 24 );
	
	end = inscribe_signed_r< 10 >( -9223372036854775807 - 1, buffer );
	
	EXPECT_CMP( buffer, 24, "-9223372036854775808xxxx", 24 );
}

int main( int argc, const char *const *argv )
{
	tap::start( PROGRAM, n_tests );
	
	magnitude();
	fill();
	inscribe_unsigned();
	inscribe_signed();
	
	return 0;
}

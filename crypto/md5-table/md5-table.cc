/*
	md5-table.cc
	------------
*/

/*
	This program generates the following data:
	
	d76aa478 e8c7b756 242070db c1bdceee f57c0faf 4787c62a a8304613 fd469501
	698098d8 8b44f7af ffff5bb1 895cd7be 6b901122 fd987193 a679438e 49b40821
	f61e2562 c040b340 265e5a51 e9b6c7aa d62f105d 02441453 d8a1e681 e7d3fbc8
	21e1cde6 c33707d6 f4d50d87 455a14ed a9e3e905 fcefa3f8 676f02d9 8d2a4c8a
	fffa3942 8771f681 6d9d6122 fde5380c a4beea44 4bdecfa9 f6bb4b60 bebfbc70
	289b7ec6 eaa127fa d4ef3085 04881d05 d9d4d039 e6db99e5 1fa27cf8 c4ac5665
	f4292244 432aff97 ab9423a7 fc93a039 655b59c3 8f0ccc92 ffeff47d 85845dd1
	6fa87e4f fe2ce6e0 a3014314 4e0811a1 f7537e82 bd3af235 2ad7d2bb eb86d391
	
	... using this command to unpack and print it:
	
		md5-table | perl -e 'read STDIN, $_, 256; $_ = unpack "H*";
			s/(.{64})/\t\t$1\n/g; s/(\S{8})/$1 /g; s/ \n/\n/g; print'
	
	See RFC 1321, "The MD5 Message-Digest Algorithm".
	<http://www.faqs.org/rfcs/rfc1321.html>
*/

// POSIX
#include <unistd.h>

// Standard C
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

// iota
#include "iota/endian.hh"


int main()
{
	uint32_t data[ 64 ];
	
	for ( int i = 0;  i < 64;  ++i )
	{
		uint32_t x = uint32_t( 4294967296.0 * fabs( sin( i + 1.0 ) ) );
		
		data[ i ] = iota::big_u32( x );
	}
	
	ssize_t n_written = write( STDOUT_FILENO, data, sizeof data );
	
	if ( n_written != sizeof data )
	{
		abort();
	}
	
	return 0;
}

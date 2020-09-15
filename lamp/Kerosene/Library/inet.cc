/*
	inet.cc
	-------
*/

// POSIX
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

// Standard C
#include <errno.h>


static void inscribe_digits( unsigned char x, char*& dst )
{
	if ( x >= 10 )
	{
		if ( x >= 100 )
		{
			const unsigned char hundreds = x / 100;
			
			x %= 100;
			
			*dst++ = '0' + hundreds;
		}
		
		const unsigned char tens = x / 10;
		
		x %= 10;
		
		*dst++ = '0' + tens;
	}
	
	*dst++ = '0' + x;
}

const char* inet_ntop( int af, const void* src, char* dst, socklen_t size )
{
	if ( af != AF_INET )
	{
		errno = EAFNOSUPPORT;
		
		return 0;  // NULL
	}
	
	if ( size < INET_ADDRSTRLEN )
	{
		errno = ENOSPC;
		
		return 0;  // NULL
	}
	
	const unsigned char* addr = (const unsigned char*) src;
	
	char* p = dst;
	
	inscribe_digits( *addr++, p );  *p++ = '.';
	inscribe_digits( *addr++, p );  *p++ = '.';
	inscribe_digits( *addr++, p );  *p++ = '.';
	inscribe_digits( *addr,   p );  *p   = '\0';
	
	return dst;
}

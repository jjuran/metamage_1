/*
	CGImageFromURL.cc
	-----------------
*/

#include "CGImageFromURL.hh"

// POSIX
#include <fcntl.h>
#include <sys/stat.h>

// CGQuickDraw
#include "CGQuickDraw.hh"


static
CGImageRef CreateCGImageFromV8R8( const char* data )
{
	enum
	{
		smear    = 2,
		width    = 256,
		height   = 256 + smear,
		rowBytes = width / 8,  // 32 bytes
	};
	
	char buffer[ height * rowBytes ] = {};
	
	char* p = buffer;
	
	int b = 0;
	
	for ( int i = 0;  i < width;  ++i )
	{
		b = b - 1 & 0x7;
		
		int mask = 1 << b;
		
		unsigned c = (Byte) ~data[ i ];
		
		for ( int j = 0;  j < smear + 1;  ++j )
		{
			p[ rowBytes * c++ ] |= mask;
		}
		
		p += b == 0;
	}
	
	BitMap bitmap = { buffer, rowBytes, { 0, 0, height, width } };
	
	return CreateCGImageFromBitMap( bitmap );
}

static
CGImageRef CGImageCreateFromPath( const char* path )
{
	CGImageRef image = NULL;
	
	int fd = open( path, O_RDONLY | O_NONBLOCK );
	
	if ( fd < 0 )
	{
		return NULL;
	}
	
	struct stat st;
	
	int nok = fstat( fd, &st );
	
	char buffer[ 4096 ];
	
	if ( nok == 0  &&  S_ISREG( st.st_mode )  &&  st.st_size <= sizeof buffer )
	{
		ssize_t n = read( fd, buffer, sizeof buffer );
		
		switch ( n )
		{
			case 256:  image = CreateCGImageFromV8R8( buffer );  break;
			
			default:
				break;
		}
	}
	
	close( fd );
	
	return image;
}

CGImageRef CGImageCreateFromURL( CFURLRef url )
{
	const CFStringEncoding encoding = kCFStringEncodingUTF8;
	
	CFStringRef string = CFURLCopyFileSystemPath( url, kCFURLPOSIXPathStyle );
	
	const char* path = CFStringGetCStringPtr( string, encoding );
	
	char buffer[ PATH_MAX ];
	
	if ( path == NULL )
	{
		if ( ! CFStringGetCString( string, buffer, sizeof buffer, encoding ) )
		{
			return NULL;
		}
		
		path = buffer;
	}
	
	CGImageRef image = CGImageCreateFromPath( path );
	
	CFRelease( string );
	
	return image;
}

/*
	CreateImage.cc
	--------------
*/

#include "CreateImage.hh"

// POSIX
#include <fcntl.h>
#include <sys/stat.h>

// bits
#include "bits/bits.hh"

// mac-cg-utils
#include "mac_cg/images.hh"


static
CGImageRef CreateImageFromPath( const char* path )
{
	using mac::cg::create_gray_paint_image;
	
	CGImageRef image = NULL;
	
	int fd = open( path, O_RDONLY | O_NONBLOCK );
	
	if ( fd < 0 )
	{
		return NULL;
	}
	
	struct stat st;
	
	int nok = fstat( fd, &st );
	
	size_t size = st.st_size;
	
	void* buffer;
	
	if ( nok == 0  &&  S_ISREG( st.st_mode )  &&  (buffer = malloc( size )) )
	{
		ssize_t n = read( fd, buffer, size );
		
		const bits::header& bits = *(const bits::header*) buffer;
		
		if ( n == size  &&  valid( bits, size ) )
		{
			const int stride = bits.stride;
			const int height = bits.height;
			const int width  = bits.width;
			
			char* p = (char*) buffer + sizeof (bits::header);
			
			image = create_gray_paint_image( width, height, 1, stride, p );
		}
		
		free( buffer );
	}
	
	close( fd );
	
	return image;
}

CGImageRef CreateImageFromURL( CFURLRef url )
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
	
	CGImageRef image = CreateImageFromPath( path );
	
	CFRelease( string );
	
	return image;
}

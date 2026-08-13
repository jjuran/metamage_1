/*
	img2bits.cc
	-----------
*/

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// POSIX
#include <fcntl.h>
#include <unistd.h>

// Standard C
#include <errno.h>
#include <string.h>

// more-libc
#include "more/string.h"

// more-posix
#include "more/perror.hh"

// bits
#include "bits/bits.hh"


#define PROGRAM  "img2bits"

#define MISSING_INPUT   "input file required"

#define STR_LEN( s )  "" s, (sizeof s - 1)


static inline
void report_error( const char* path, uint32_t err )
{
	more::perror( PROGRAM, path, err );
}

static
CGImageRef create_image_from_src( const char* path )
{
	CFURLRef url;
	url = CFURLCreateFromFileSystemRepresentation( NULL,
	                                               (const uint8_t*) path,
	                                               strlen( path ),
	                                               false );
	
	if ( url == NULL )
	{
		report_error( "CFURLCreateFromFileSystemRepresentation", errno );
		
		exit( 1 );
	}
	
	CGImageSourceRef src = CGImageSourceCreateWithURL( url, NULL );
	
	CFRelease( url );
	
	if ( src == NULL )
	{
		report_error( path, errno );
		
		exit( 1 );
	}
	
	CGImageRef image = CGImageSourceCreateImageAtIndex( src, 0, NULL );
	
	CFRelease( src );
	
	if ( image == NULL )
	{
		report_error( "CGImageSourceCreateImageAtIndex", errno );
		
		exit( 1 );
	}
	
	return image;
}

static
void downsample_8_to_1( const Byte* src_1px, Byte* dst_8px, int n_dst_bytes )
{
	while ( n_dst_bytes-- > 0 )
	{
		Byte octet = 0;
		
		for ( int i = 0;  i < 8;  ++i )
		{
			octet <<= 1;
			
			octet |= (*src_1px++ & 0x80) >> 7;  // src pixels are any value
		}
		
		*dst_8px++ = ~octet;
	}
}

int main( int argc, char** argv )
{
	const char* output_path = NULL;
	
	char** args = argv + 1;
	
	const char* src = *args;
	
	if ( src == NULL )
	{
		write( STDERR_FILENO, STR_LEN( PROGRAM ": " MISSING_INPUT "\n" ) );
		
		return 2;
	}
	
	if ( argc >= 3  &&  strcmp( *args, "-o" ) == 0 )
	{
		++args;
		output_path = *args++;
		
		src = *args;
	}
	else
	{
		size_t len = strlen( src );
		
		void* p = alloca( len + sizeof ".bits" );
		
		output_path = (char*) p;
		
		p = mempcpy( p, src,     len            );
		p = mempcpy( p, ".bits", sizeof ".bits" );  // includes final NUL byte
	}
	
	CGImageRef image = create_image_from_src( src );
	
	size_t width  = CGImageGetWidth ( image );
	size_t height = CGImageGetHeight( image );
	
	size_t bytes_per_row = CGImageGetBytesPerRow( image );
	
	size_t total_bytes = sizeof (bits::header) + bytes_per_row * height;
	
	void* alloc = malloc( total_bytes );
	
	if ( alloc == NULL )
	{
		report_error( "malloc", errno );
		
		exit( 1 );
	}
	
	memset( alloc, '\xff', total_bytes );
	
	bits::header* bits = (bits::header*) alloc;
	
	Byte* data = (Byte*) &bits[ 1 ];
	
	size_t bits_per_component = 8;
	
	CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceGray();
	
	CGContextRef c = CGBitmapContextCreate( data,
	                                        width,
	                                        height,
	                                        bits_per_component,
	                                        bytes_per_row,
	                                        colorSpace,
	                                        kCGImageAlphaNone );
	
	if ( c == NULL )
	{
		report_error( "CGBitmapContextCreate", errno );
		
		exit( 1 );
	}
	
	CGColorSpaceRelease( colorSpace );
	
	CGRect rect = CGRectMake( 0, 0, width, height );
	
	CGContextDrawImage( c, rect, image );
	
	CGImageRelease( image );
	
	CGContextRelease( c );
	
	Byte* p = (Byte*) data;
	Byte* q = (Byte*) data;
	
	short rowBytes = (width + 15) >> 4 << 1;
	
	for ( int i = 0;  i < height;  ++i )
	{
		downsample_8_to_1( p, q, rowBytes );
		
		p += bytes_per_row;
		q += rowBytes;
	}
	
	int fd = open( output_path, O_WRONLY | O_CREAT | O_TRUNC, 0666 );
	
	if ( fd < 0 )
	{
		report_error( output_path, errno );
		
		return 1;
	}
	
	bits->magic  = bits::magic;
	bits->stride = rowBytes;
	bits->height = height;
	bits->width  = width;
	
	total_bytes = sizeof (bits::header) + rowBytes * height;
	
	ssize_t wrote = write( fd, alloc, total_bytes );
	
	if ( wrote < 0 )
	{
		report_error( output_path, errno );
		
		return 1;
	}
	
	int closed = close( fd );
	
	if ( closed < 0 )
	{
		report_error( output_path, errno );
		
		return 1;
	}
	
	return 0;
}

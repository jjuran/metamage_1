/*
	utils.cc
	--------
*/

#include "icongen/utils.hh"

// Standard C
#include <stdio.h>
#include <stdlib.h>

// icongen
#include "icongen/thumbnail.hh"


static
size_t round_up_to_x16( size_t x )
{
	return (x + 15) & ~15;
}

CGContextRef create_bitmap_context( size_t length )
{
	const size_t width  = length;
	const size_t height = length;
	
	const size_t bits_per_component = 8;
	const size_t bytes_per_row      = round_up_to_x16( width * 4 );
	
	const size_t total_bytes = bytes_per_row * height;
	
	void* bytes = malloc( total_bytes );
	
	if ( bytes == NULL )
	{
		fprintf( stderr, "malloc() returned NULL\n" );
		exit( 1 );
	}
	
	CGColorSpaceRef colorSpace;
	
	colorSpace = CGColorSpaceCreateWithName( kCGColorSpaceGenericRGB );
	
	if ( colorSpace == NULL )
	{
		fprintf( stderr, "CGColorSpaceCreateWithName() returned NULL\n" );
		exit( 1 );
	}
	
	CGBitmapInfo bitmapInfo = kCGImageAlphaPremultipliedFirst;
	
	CGContextRef c = CGBitmapContextCreate( bytes,
	                                        width,
	                                        height,
	                                        bits_per_component,
	                                        bytes_per_row,
	                                        colorSpace,
	                                        bitmapInfo );
	
	CGColorSpaceRelease( colorSpace );
	
	if ( c == NULL )
	{
		fprintf( stderr, "CGBitmapContextCreate() returned NULL\n" );
		exit( 1 );
	}
	
	return c;
}

static
void write_image_as_type( CGContextRef c, const char* path, CFStringRef type )
{
	CGImageRef image = CGBitmapContextCreateImage( c );
	
	if ( image == NULL )
	{
		fprintf( stderr, "CGBitmapContextCreateImage() returned NULL\n" );
		exit( 1 );
	}
	
	CFURLRef url;
	url = CFURLCreateFromFileSystemRepresentation( NULL,
	                                               (const UInt8*) path,
	                                               strlen( path ),
	                                               false );
	
	if ( url == NULL )
	{
		fprintf( stderr, "CFURLCreateFromFileSystemRepresentation() returned NULL\n" );
		exit( 1 );
	}
	
	CGImageDestinationRef dest;
	dest = CGImageDestinationCreateWithURL( url, type, 1, NULL );
	
	CFRelease( url );
	
	if ( dest == NULL )
	{
		fprintf( stderr, "CGImageDestinationCreateWithURL() returned NULL\n" );
		exit( 1 );
	}
	
	CGImageDestinationAddImage( dest, image, NULL );
	
	CGImageRelease( image );
	
	CGImageDestinationFinalize( dest );
	
	CFRelease( dest );
}

void write_PNG_image( CGContextRef c, const char* path )
{
	write_image_as_type( c, path, kUTTypePNG );
}

void write_JPEG_2000( CGContextRef c, const char* path )
{
	write_image_as_type( c, path, kUTTypeJPEG2000 );
}

static
void write_buffer_to_file( const void* buffer, size_t n, const char* path )
{
	int fd = open( path, O_WRONLY | O_CREAT | O_TRUNC, 0666 );
	
	if ( fd < 0 )
	{
		perror( path );
		exit( 1 );
	}
	
	ssize_t n_written = write( fd, buffer, n );
	
	if ( n_written != n )
	{
		perror( path );
		exit( 1 );
	}
	
	int nok = close( fd );
	
	if ( nok < 0 )
	{
		perror( path );
		exit( 1 );
	}
}

typedef Byte* (*encoder)( size_t, size_t, size_t, const Byte*, Byte* );

static
void write_thumbnail_data( CGContextRef c, const char* path, encoder encode )
{
	void* data = CGBitmapContextGetData( c );
	
	const size_t width  = CGBitmapContextGetWidth ( c );
	const size_t height = CGBitmapContextGetHeight( c );
	
	const size_t bytes_per_row = CGBitmapContextGetBytesPerRow( c );
	
	const size_t buffer_size = thumbnail_buffer_size( width, height );
	
	void* buffer = malloc( buffer_size );
	
	Byte const* src = (Byte*) data;
	Byte*       dst = (Byte*) buffer;
	
	dst = encode( width, height, bytes_per_row, src, dst );
	
	const size_t n = dst - (Byte*) buffer;
	
	write_buffer_to_file( buffer, n, path );
	
	free( buffer );
}

void write_thumbnail( CGContextRef c, const char* path )
{
	write_thumbnail_data( c, path, &copy_rle_thumbnail );
}

void write_mask( CGContextRef c, const char* path )
{
	write_thumbnail_data( c, path, &copy_thumbnail_plane );
}

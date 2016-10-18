/*
	utils.cc
	--------
*/

#include "icongen/utils.hh"

// Standard C
#include <stdio.h>
#include <stdlib.h>


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

void write_PNG_image( CGContextRef c, const char* path )
{
	CGImageRef image = CGBitmapContextCreateImage( c );
	
	if ( image == NULL )
	{
		fprintf( stderr, "CGBitmapContextCreateImage() returned NULL\n" );
		exit( 1 );
	}
	
	CFURLRef url;
	url = CFURLCreateFromFileSystemRepresentation( NULL,
	                                               (const uint8_t*) path,
	                                               strlen( path ),
	                                               false );
	
	if ( url == NULL )
	{
		fprintf( stderr, "CFURLCreateFromFileSystemRepresentation() returned NULL\n" );
		exit( 1 );
	}
	
	CGImageDestinationRef dest;
	dest = CGImageDestinationCreateWithURL( url, kUTTypePNG, 1, NULL );
	
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

/*
	skif2png.cc
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
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// iota
#include "iota/endian.hh"

// more-posix
#include "more/perror.hh"

// raster
#include "raster/load.hh"
#include "raster/skif.hh"


#define PROGRAM  "skif2png"

#define MISSING_INPUT   "input file required"
#define MISSING_OUTPUT  "stdout is a tty, either redirect or use -o"

#define STR_LEN( s )  "" s, (sizeof s - 1)


using namespace raster;


static inline
void report_error( const char* path, uint32_t err )
{
	more::perror( PROGRAM, path, err );
}

static
void release_data( void* info, const void* data, size_t size )
{
	::operator delete( const_cast< void* >( data ) );
}

typedef void (*copier)( void* dst, const void* src, size_t n );

static
void straight_copy( void* dst, const void* src, size_t n )
{
	memcpy( dst, src, n );
}

static
void inverted_copy( void* dst, const void* src, size_t n )
{
	uint32_t const* p   = (const uint32_t*) src;
	uint32_t const* end = (const uint32_t*) src + n / 4;
	
	uint32_t* q = (uint32_t*) dst;
	
	while ( p < end )
	{
		*q++ = ~*p++;
	}
}

static
void converting_LE_565_to_555_copy( void* dst, const void* src, size_t n )
{
	uint16_t const* p   = (const uint16_t*) src;
	uint16_t const* end = (const uint16_t*) src + n / 2;
	
	uint16_t* q = (uint16_t*) dst;
	
	while ( p < end )
	{
		uint16_t pixel = iota::u16_from_little( *p++ );
		
		uint16_t red_green = pixel & 0xFFC0;  // upper 10 bits
		uint16_t blue      = pixel & 0x001F;  // lower 5 bits
		
		pixel = (red_green >> 1) | blue;
		
		*q++ = iota::little_u16( pixel );
	}
}

static
CGDataProviderRef make_data_provider( char* data, size_t size, copier cpy )
{
	void* buffer = ::operator new( size );
	
	cpy( buffer, data, size );
	
	return CGDataProviderCreateWithData( NULL,
	                                     buffer,
	                                     size,
	                                     &release_data );
}

static
CGImageRef CGImage_from_raster( const raster_load& raster )
{
	const raster_desc& desc = raster.meta->desc;
	
	char* base = (char*) raster.addr;
	
	bool little_endian = *(uint16_t*) (base + raster.size - 4);
	
	size_t width  = desc.width;
	size_t height = desc.height;
	size_t weight = desc.weight;
	size_t stride = desc.stride;
	
	const bool tri_colored = desc.model >= Model_RGB;
	
	size_t bits_per_component = weight == 32 ? 8
	                          : weight == 16 ? 5
	                          :                weight;
	
	int n_components = tri_colored ? 3 : 1;
	
	CFStringRef colorSpaceName = tri_colored ? kCGColorSpaceGenericRGB
	                                         : kCGColorSpaceGenericGray;
	
	CGColorSpaceRef colorSpace = CGColorSpaceCreateWithName( colorSpaceName );
	
	CGBitmapInfo bitmapInfo = kCGImageAlphaNoneSkipFirst;
	
	if ( desc.model > Model_RGB  &&  desc.model <= Model_RGBA_premultiplied )
	{
		bitmapInfo = Model_RGBA_premultiplied + 1 - desc.model;
	}
	else if ( desc.model == Model_RGB  &&  desc.magic == kSKIFFileType )
	{
		if ( weight == 32 )
		{
			switch ( desc.layout.per_pixel )
			{
				default:
				case xRGB:
					break;
				
				case ARGB:
					bitmapInfo = kCGImageAlphaFirst;
					break;
				
				case RGBx:
				case xBGR:  // (big-endian RGBx in little-endian container)
					bitmapInfo = kCGImageAlphaNoneSkipLast;
					break;
				
				case RGBA:
				case ABGR:  // (big-endian RGBA in little-endian container)
					bitmapInfo = kCGImageAlphaLast;
					break;
			}
			
			if ( (uint8_t) desc.layout.per_pixel == Channel_red )
			{
				/*
					Big-endian RGBA (or RGBx) data in a little-endian file.
					Quartz has no concept of ABGR, so clear the flag instead.
				*/
				
				little_endian = false;
			}
		}
	}
	
	if ( little_endian )
	{
		bitmapInfo |= weight == 16 ? kCGBitmapByteOrder16Little
		            : weight == 32 ? kCGBitmapByteOrder32Little
		            :                kCGBitmapByteOrderDefault;
	}
	
	copier cpy = desc.model == Model_monochrome_paint ? inverted_copy
	                                                  : straight_copy;
	
	if ( little_endian  &&  weight == 16  &&  is_16bit_565( desc ) )
	{
		/*
			This is a lossy conversion, as it drops the rightmost green bit.
			TODO:  Convert to 8/8/8/8 instead.
		*/
		
		cpy = &converting_LE_565_to_555_copy;
	}
	
	CGDataProviderRef dataProvider = make_data_provider( base,
	                                                     height * stride,
	                                                     cpy );
	
	CGImageRef image;
	image = CGImageCreate( width,
	                       height,
	                       bits_per_component,
	                       weight,
	                       stride,
	                       colorSpace,
	                       bitmapInfo,
	                       dataProvider,
	                       NULL,
	                       false,
	                       kCGRenderingIntentDefault );
	
	CFRelease( dataProvider );
	CGColorSpaceRelease( colorSpace );
	
	return image;
}

static
void write_image_as_type( CGImageRef image, const char* path, CFStringRef type )
{
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
	dest = CGImageDestinationCreateWithURL( url, type, 1, NULL );
	
	CFRelease( url );
	
	if ( dest == NULL )
	{
		fprintf( stderr, "CGImageDestinationCreateWithURL() returned NULL\n" );
		exit( 1 );
	}
	
	CGImageDestinationAddImage( dest, image, NULL );
	
	CGImageDestinationFinalize( dest );
	
	CFRelease( dest );
}

static
void raster_to_PNG( const raster_load& raster, const char* path )
{
	CGImageRef image = CGImage_from_raster( raster );
	
	write_image_as_type( image, path, kUTTypePNG );
	
	CGImageRelease( image );
}

int main( int argc, char** argv )
{
	const char* output_path = "/dev/fd/1";
	
	char** args = argv + 1;
	
	if ( argc >= 3  &&  strcmp( *args, "-o" ) == 0 )
	{
		++args;
		output_path = *args++;
	}
	else if ( isatty( STDOUT_FILENO ) )
	{
		write( STDERR_FILENO, STR_LEN( PROGRAM ": " MISSING_OUTPUT "\n" ) );
		return 2;
	}
	
	const char* src = *args;
	
	if ( src == NULL )
	{
		write( STDERR_FILENO, STR_LEN( PROGRAM ": " MISSING_INPUT "\n" ) );
		return 2;
	}
	
	int fd = open( src, O_RDONLY );
	
	if ( fd < 0 )
	{
		report_error( src, errno );
		return 1;
	}
	
	raster_load raster = load_raster( fd );
	
	if ( raster.addr == NULL )
	{
		report_error( src, errno );
		return 1;
	}
	
	raster_to_PNG( raster, output_path );
	
	close( fd );
	
	return 0;
}

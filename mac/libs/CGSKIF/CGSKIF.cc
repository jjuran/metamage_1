/*
	CGSKIF.cc
	---------
*/

#include "CGSKIF.hh"

// Standard C
#include <stdlib.h>

// iota
#include "iota/endian.hh"

// mac-cg-utils
#include "mac_cg/colorspaces.hh"
#include "mac_cg/data.hh"

// rasterlib
#include "raster/clut.hh"
#include "raster/clut_detail.hh"
#include "raster/skif.hh"


#ifndef MAC_OS_X_VERSION_10_4
typedef CGImageAlphaInfo CGBitmapInfo;
#endif


using namespace raster;


static
void convert_LE_565_to_555( void* dst, const void* src, size_t n )
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

CGImageRef CGSKIFCreateImageFromRaster( const raster_load& raster )
{
	using mac::cg::create_inverted_grayscale;
	using mac::cg::create_RGB_palette;
	using mac::cg::generic_or_device_gray;
	using mac::cg::generic_or_device_RGB;
	
	const clut_data* clut = NULL;
	
	const raster_desc& desc = raster.meta->desc;
	
	char* base = (char*) raster.addr;
	
	bool little_endian = *(uint16_t*) (base + raster.size - 4);
	
	size_t width  = desc.width;
	size_t height = desc.height;
	size_t weight = desc.weight;
	size_t stride = desc.stride;
	
	bool tri_colored = desc.model > Model_palette;
	
	if ( desc.model == Model_palette )
	{
		clut = find_clut( &raster.meta->note );
		
		if ( clut != NULL )
		{
			tri_colored = true;
		}
	}
	
	size_t bits_per_component = weight == 32 ? 8
	                          : weight == 16 ? 5
	                          :                weight;
	
	int n_components = tri_colored ? 3 : 1;
	
	CGColorSpaceRef colorSpace = tri_colored ? generic_or_device_RGB()
	                                         : generic_or_device_gray();
	
	CGColorSpaceRef indexed = NULL;
	
	if ( desc.model == Model_monochrome_paint )
	{
		indexed = create_inverted_grayscale( 1 << weight );
	}
	
	if ( clut != NULL )
	{
		const UInt16* colors = (const UInt16*) clut->palette;
		
		indexed = create_RGB_palette( colors, clut->max + 1 );
	}
	
	if ( indexed )
	{
		colorSpace = indexed;
	}
	
	CGBitmapInfo bitmapInfo = kCGImageAlphaNone;
	
	if ( desc.model > Model_RGB  &&  desc.model <= Model_RGBA_premultiplied )
	{
		bitmapInfo = CGBitmapInfo( Model_RGBA_premultiplied + 1 - desc.model );
	}
	else if ( desc.model == Model_RGB  &&  desc.magic == kSKIFFileType )
	{
		bitmapInfo = kCGImageAlphaNoneSkipFirst;
		
		if ( weight == 32 )
		{
			switch ( desc.layout.per_pixel )
			{
				default:
				case xRGB:
				case BGRx:
//					bitmapInfo = kCGImageAlphaNoneSkipFirst;
					break;
				
				case ARGB:
				case BGRA:
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
			
			if ( (Byte) desc.layout.per_pixel == Channel_red )
			{
				/*
					Big-endian RGBA (or RGBx) data in a little-endian file.
					Quartz has no concept of ABGR, so clear the flag instead.
				*/
				
				little_endian = false;
			}
		}
	}
	
	using mac::cg::make_data_provider_copy;
	using mac::cg::make_data_provider_xfer;
	
	CGDataProviderRef dataProvider;
	
	if ( little_endian  &&  weight == 16  &&  is_16bit_565( desc ) )
	{
		bitmapInfo = kCGImageAlphaNoneSkipFirst;
		
		/*
			This is a lossy conversion, as it drops the rightmost green bit.
			TODO:  Convert to 8/8/8/8 instead.
		*/
		
		size_t size = height * stride;
		
		void* converted = malloc( size );
		
		if ( ! converted )
		{
			return NULL;
		}
		
		convert_LE_565_to_555( converted, base, size );
		
		dataProvider = make_data_provider_xfer( converted, size );
	}
	else
	{
		dataProvider = make_data_provider_copy( base, height * stride );
	}
	
#ifdef MAC_OS_X_VERSION_10_4
	
	if ( little_endian )
	{
		bitmapInfo |= weight == 16 ? kCGBitmapByteOrder16Little
		            : weight == 32 ? kCGBitmapByteOrder32Little
		            :                kCGBitmapByteOrderDefault;
	}
	
#endif
	
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
	
	if ( indexed )
	{
		CGColorSpaceRelease( indexed );
	}
	
	return image;
}

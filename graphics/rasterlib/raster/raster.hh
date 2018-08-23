/*
	raster.hh
	---------
*/

#ifndef RASTER_RASTER_HH
#define RASTER_RASTER_HH

// Standard C
#include <stddef.h>
#include <stdint.h>

// raster
#include "raster/note.hh"


namespace raster
{
	
	/*
		Raster metaformat 0
		
		A classic Macintosh screen image (a.k.a. StartupScreen).  There's no
		metadata structure -- a file is identified as type 0 if and only if
		it's 21888 bytes long.  The `magic` field in the raster_desc returned
		by load_raster() is set to 'SCRN', and the `version` field is set to 0.
		No synchronization is possible.
		
		Dimensions: 512x342, stride: 64 bytes, pixel weight: 1 bit.
		Model: monochrome paint (white = 0; black = 1).
	*/
	
	/*
		Raster metaformat 1
		
		A metadata footer is located at the end of the file.  The file length
		must be a multiple of 4 bytes.  The last 4 bytes contain the size of
		the footer.  The size must be less than 65536 and nonzero; therefore
		either endianness can be used unambiguously.  The image data must be
		contiguous and uncompressed.  This rather limits the ability to tack
		a footer onto an image file, but it's possible to do so with TIFF, at
		least.
		
		Subtract the footer size from the file size to get the footer offset.
		
		(Typically, a footer will be made large enough to extend the file
		size to a multiple of the disk block size.)
	*/
	
	/*
		struct raster_desc
		
		magic, version:
			Early raster files have zero in both fields.  Since they have zero
			magic, let us call them *mundane* rasters.
			
			SKIF (Simple Kinetic Image Format) is a refinement.  Its magic
			value is 'SKIF'.  Set the version to zero for now.
		
		width, height:
			Image dimensions in pixels.  While `height` is the number of rows
			in the image, and `width` is the number of pixels per row, rows
			may have extra bits on the right that aren't used for pixels --
			so `width` can't be used directly in byte size calculations.
		
		stride:
			The memory size of a row in bytes (including any unused bits).
			The image's memory size is `height * stride`.
		
		weight:
			The memory size of a pixel in bits, which must be a factor or
			multiple of 8.  The weight includes any unused bits, so 0/5/5/5
			has a weight of 16, not 15, and xRGB's weight is 32, not 24.
		
		model:
			A selector indicating what the bits in a pixel mean, similar in
			purpose to TIFF's PhotometricInterpretation.
			
			For mundane rasters:
			0:  Monochrome "paint".  Pixels with all bits set to zero are white;
			    those with all bits set to one are black.  Values in between
			    map to a linearly interpolated gray color.
			    This corresponds with a TIFF PhotometricInterpretation value
			    of WhiteIsZero.
			1:  Monochrome "light".  Pixels with all bits set to zero are black;
			    those with all bits set to one are white.  Values in between
			    map to a linearly interpolated gray color.
			    This corresponds with a TIFF PhotometricInterpretation value
			    of BlackIsZero.
			2:  Palette.  Each pixel value is an index into a lookup table of
			    actual color values.
			    (TODO:  Palettes aren't supported yet.)
			3:  RGB (red/green/blue).  Pixels are divided into 3 groups of bits
			    corresponding to red, green, and blue light intensities.  There
			    are no unused bits in a pixel; some color components might have
			    more bits than others (e.g. 16-bit 5/6/5, a Linux default).
			4:  xRGB (unused/red/green/blue).  Pixels are split into 4 groups
			    of bits.  The first group is ignored.  The remaining bits are
			    divided evenly among red, green, and blue.  A color component
			    might have more bits than the unused group (e.g. 1/5/5/5, used
			    in Mac OS for 16-bit color).
			5:  RGBx (red/green/blue/unused).  As above, but the unused bits
			    are last rather than first.
			6:  ARGB (alpha/red/green/blue).  Pixels are divided into 4 groups:
			    alpha, which indicates opacity, and red/green/blue as above.
			7:  RGBA (red/green/blue/alpha).  As above, but the alpha component
			    is last rather than first.
			8:  Premultiplied ARGB.  Pixels contain alpha, red, green, and blue
			    components as above, except that the color components represent
			    the result of multiplying the source value by the alpha value.
			9:  Premultiplied RGBA.  As above, but the alpha component is last
			    rather than first.
			
			For SKIF files, the meaning of values 0 through 2 above is
			unchanged.  Value 3 is RGB in general (potentially with alpha).
			The pixel layout details for RGB are defined by `layout`.
			
			All other values are reserved.
		
		extra:
			The number of additional image frames stored in this file.  A value
			of zero means one frame total; a value of 255 indicates the maximum
			frame count of 256 frames.
		
		frame:
			The currently displayed image frame number, zero-indexed.  Must be
			less than or equal to `extra`.
		
		layout:
			The pixel format description for model RGB in SKIF files.  For a
			pixel weight of 32, it specifies the function of each byte in a
			pixel.  For a pixel weight of 8 or 16, it provides the location of
			each color channel within a pixel as length and offset (in bits).
			For any other pixel weight, it's unused and should be set to zero.
			
			See "Pixel format specifiers", below.
	*/
	
	/*
		Pixel format specifiers
		
		For pixel weight of 8 or 16:
		
			bit count  (4 bits)
			bit index  (4 bits)
		
		More significant bits are to the left of less significant bits.
		
		Examples:
			3/3/2:    red=0x35, green=0x32, blue=0x20, alpha=0x00
			5/6/5:    red=0x5B, green=0x65, blue=0x50, alpha=0x00
			1/5/5/5:  red=0x5A, green=0x55, blue=0x50, alpha=0x1F
		
		Little-endian note:  Multibyte pixels are in native byte order.
		For little-endian files, the pairs of bytes forming each pixel
		are swapped, so a fully saturated 5/6/5 green pixel would appear
		in the file as 0xE0 0x07, not 0x07 0xE0.
		
		For pixel weight of 32:
		
			xRGB 32:  0 1 2 3
			ARGB 32:  4 1 2 3
			RGBA 32:  1 2 3 4
			BGRA 32:  3 2 1 4
			ABGR 32:  4 3 2 1
			Argb 32:  4 5 6 7 (premultiplied)
		
		Little-endian note:  Multibyte pixels and layout.per_pixel are in native
		byte order.  For little-endian files, the quadruplets of bytes forming
		each pixel are swapped, and the bytes of the pixel layout descriptor
		are /also/ swapped.  Therefore, a program can assign layout.per_pixel
		the value 0x04010203 (ARGB) and write pixels as 4-byte integers, and
		the results will be equivalent regardless of whether the architecture
		is big-endian or little-endian.  In the latter case, the actual bytes
		in the file will be x"03020104", and the pixels will be in BGRA order.
		
	*/
	
	enum raster_model
	{
		Model_none = -1,
		
		Model_monochrome_paint,
		Model_monochrome_light,
		Model_palette,
		Model_RGB,
		
		// for "mundane" rasters only:
		Model_xRGB,
		Model_RGBx,
		Model_ARGB,
		Model_RGBA,
		Model_ARGB_premultiplied,
		Model_RGBA_premultiplied,
		
		Model_end_of_enumeration,
	};
	
	enum data_channel
	{
		Channel_unused,
		
		Channel_red,
		Channel_green,
		Channel_blue,
		
		Channel_alpha,
		
		Channel_premultiplied_red,
		Channel_premultiplied_green,
		Channel_premultiplied_blue,
	};
	
	union pixel_layout
	{
		// For weight = 32:
		
		uint32_t  per_pixel;
		uint8_t   per_byte[ 4 ];
		
		// For weight = 16 (or 8):
		
		struct
		{
			uint8_t  red;
			uint8_t  green;
			uint8_t  blue;
			uint8_t  alpha;
		};
	};
	
	struct raster_desc
	{
		// meta
		uint32_t  magic;
		uint32_t  version;
		
		// dimensions
		uint32_t  width;   // pixels per row (not including gutter)
		uint32_t  height;  // rows per frame
		
		uint32_t  stride;  // bytes per row
		uint8_t   weight;  // bits per pixel
		uint8_t   model;   // raster_model
		uint8_t   extra;   // number of additional stored frames
		uint8_t   frame;   // currently displayed frame
		
		pixel_layout  layout;
		
		uint32_t  reserved;
	};
	
	struct raster_metadata
	{
		raster_desc  desc;
		raster_note  note;  // Actually, one or more notes
	};
	
	struct raster_load
	{
		void*             addr;
		size_t            size;
		raster_metadata*  meta;
	};
	
	
	inline
	const raster_note* find_note( const raster_metadata& meta, note_type type )
	{
		return find( &meta.note, type );
	}
	
	inline
	raster_note* find_note( raster_metadata& meta, note_type type )
	{
		return find( &meta.note, type );
	}
	
}

#endif

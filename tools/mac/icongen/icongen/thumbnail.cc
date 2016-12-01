/*
	thumbnail.cc
	------------
*/

#include "icongen/thumbnail.hh"

// icongen
#include "icongen/rle.hh"


uint8_t* copy_thumbnail_plane( size_t          width,
                               size_t          height,
                               size_t          bytes_per_row,
                               const uint8_t*  src,
                               uint8_t*        dst )
{
	for ( size_t i = 0;  i < height;  ++i )
	{
		const uint8_t* p = src;
		
		for ( size_t j = 0;  j < width;  ++j )
		{
			*dst++ = *p;
			
			p += 4;
		}
		
		src += bytes_per_row;
	}
	
	return dst;
}

static
uint8_t* copy_rle_thumbnail_plane( size_t          width,
                                   size_t          height,
                                   size_t          bytes_per_row,
                                   const uint8_t*  src,
                                   uint8_t*        dst )
{
	static uint8_t* plane_buffer = new uint8_t[ width * height ];
	
	copy_thumbnail_plane( width, height, bytes_per_row, src, plane_buffer );
	
	return rle_encode( dst, plane_buffer, plane_buffer + width * height );
}

uint8_t* copy_rle_thumbnail( size_t          width,
                             size_t          height,
                             size_t          bytes_per_row,
                             const uint8_t*  src,
                             uint8_t*        dst )
{
	uint32_t* null = (uint32_t*) dst;
	
	*null++ = 0;
	
	dst = (uint8_t*) null;
	
	dst = copy_rle_thumbnail_plane( width, height, bytes_per_row, ++src, dst );
	dst = copy_rle_thumbnail_plane( width, height, bytes_per_row, ++src, dst );
	dst = copy_rle_thumbnail_plane( width, height, bytes_per_row, ++src, dst );
	
	return dst;
}

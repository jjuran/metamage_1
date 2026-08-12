/*
	thumbnail.hh
	------------
*/

#ifndef ICONGEN_THUMBNAIL_HH
#define ICONGEN_THUMBNAIL_HH

// Standard C
#include <stddef.h>
#include <stdint.h>


inline
size_t thumbnail_buffer_size( size_t width, size_t height )
{
	return 4 + (1 + width) * height * 3;
}

uint8_t* copy_thumbnail_plane( size_t          width,
                               size_t          height,
                               size_t          bytes_per_row,
                               const uint8_t*  src,
                               uint8_t*        dst );

uint8_t* copy_rle_thumbnail( size_t          width,
                             size_t          height,
                             size_t          bytes_per_row,
                             const uint8_t*  src,
                             uint8_t*        dst );


#endif

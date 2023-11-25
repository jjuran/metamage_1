/*
	size.hh
	-------
*/

#ifndef RASTER_SIZE_HH
#define RASTER_SIZE_HH


namespace raster
{

inline
unsigned make_stride( unsigned width, int weight )
{
	switch ( weight )
	{
		case 1:  return (width + 15) >> 4 << 1;
		case 2:  return (width +  7) >> 3 << 1;
		case 4:  return (width +  3) >> 2 << 1;
		case 8:  return (width +  1)    & ~0x1;
		
		default:
			return width * (weight / 8u);
	}
}

inline
long good_file_size( long raster_size, long footer_size_minimum )
{
	const long disk_block_size = 512;
	const long m               = disk_block_size - 1;
	
	/*
		Round up the total file size to a multiple of the disk block size.
		The actual footer size will be (total_size - raster_size).
	*/
	
	const long total_size = (raster_size + footer_size_minimum + m) & ~m;
	
	return total_size;
}

}

#endif

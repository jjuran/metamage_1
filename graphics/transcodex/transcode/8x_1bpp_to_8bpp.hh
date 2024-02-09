/*
	transcode/8x_1bpp_to_8bpp.hh
	----------------------------
*/

#ifndef TRANSCODE_8X1BPPTO8BPP_HH
#define TRANSCODE_8X1BPPTO8BPP_HH


namespace transcode
{
	
	typedef unsigned char Byte;
	
	
	void _8x_1bpp_to_8bpp( const Byte* src, Byte* dst, int n, Byte z, Byte o );
	
	inline
	void _8x_1bpp_to_8bpp( const void* src, void* dst, int n, Byte z, Byte o )
	{
		_8x_1bpp_to_8bpp( (const Byte*) src, (Byte*) dst, n, z, o );
	}
	
}

inline
void transcode_8x_1bpp_to_8bpp( const void* src, void* dst, int n )
{
	/*
		This is actually inverted:  The 0 and 1 bits code for $FF and $00,
		respectively.  This is because this function was originally written
		for transcoding classic Mac OS (black-on-white) binary image data to
		conventional white-on-black grayscale as used in OpenGL.
	*/
	
	transcode::_8x_1bpp_to_8bpp( src, dst, n, 0xFF, 0x00 );
}

#endif

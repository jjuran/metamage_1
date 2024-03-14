/*
	transcode/8x_1bpp_to_8bpp.hh
	----------------------------
*/

#ifndef TRANSCODE_8X1BPPTO8BPP_HH
#define TRANSCODE_8X1BPPTO8BPP_HH


namespace transcode
{
	
	typedef unsigned char Byte;
	
	
	void _8x_1bpp_to_8bpp( const Byte* src, Byte* dst, int n );
	
	inline
	void _8x_1bpp_to_8bpp( const void* src, void* dst, int n )
	{
		_8x_1bpp_to_8bpp( (const Byte*) src, (Byte*) dst, n );
	}
	
}

inline
void transcode_8x_1bpp_to_8bpp( const void* src, void* dst, int n )
{
	transcode::_8x_1bpp_to_8bpp( src, dst, n );
}

#endif

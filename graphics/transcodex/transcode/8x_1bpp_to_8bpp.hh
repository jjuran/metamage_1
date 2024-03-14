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

#endif

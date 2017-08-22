/*
	Vertice/Anaglyphs.cc
	--------------------
*/

#include "Vertice/Anaglyphs.hh"


namespace Vertice
{
	
	typedef unsigned char byte;
	
	
	static
	void MergeTrueAnaglyph( const byte* left, byte* right )
	{
		double r = 0.299 * left  [1] + 0.587 * left  [2] + 0.114 * left  [3];
		double g = 0;
		double b = 0.299 * right [1] + 0.587 * right [2] + 0.114 * right [3];
		
		right[ 1 ] = (byte) r;
		right[ 2 ] = (byte) g;
		right[ 3 ] = (byte) b;
	}
	
	static
	void MergeGrayAnaglyph( const byte* left, byte* right )
	{
		double r = 0.299 * left  [1] + 0.587 * left  [2] + 0.114 * left  [3];
		double g = 0.299 * right [1] + 0.587 * right [2] + 0.114 * right [3];
		double b = g;
		
		right[ 1 ] = (byte) r;
		right[ 2 ] = (byte) g;
		right[ 3 ] = (byte) b;
	}
	
	static
	void MergeColorAnaglyph( const byte* left, byte* right )
	{
		right[1] = left[1];
	}
	
	static
	void MergeHalfColorAnaglyph( const byte* left, byte* right )
	{
		right[1] = byte( 0.299 * left[1] + 0.587 * left[2] + 0.114 * left[3] );
	}
	
	static
	void MergeOptimizedAnaglyph( const byte* left, byte* right )
	{
		right[1] = byte( 0.7 * left[2] + 0.3 * left[3] );
	}
	
	typedef void (*anaglyphic_merge_proc)( const byte*, byte* );
	
	static
	anaglyphic_merge_proc select_anaglyphic_merge( anaglyph_mode mode )
	{
		anaglyphic_merge_proc merge;
		
		switch ( mode )
		{
			case kTrueAnaglyph:
				merge = &MergeTrueAnaglyph;
				break;
			
			case kGrayAnaglyph:
				merge = &MergeGrayAnaglyph;
				break;
			
			case kColorAnaglyph:
				merge = &MergeColorAnaglyph;
				break;
			
			case kHalfColorAnaglyph:
				merge = &MergeHalfColorAnaglyph;
				break;
			
			case kOptimizedAnaglyph:
				merge = &MergeOptimizedAnaglyph;
				break;
			
			default:
				merge = NULL;
		}
		
		return merge;
	}
	
	void merge_anaglyph( anaglyph_mode  mode,
	                     void const*    left,
	                     void*          right,
	                     size_t         height,
	                     size_t         width,
	                     size_t         stride )
	{
		anaglyphic_merge_proc merge = select_anaglyphic_merge( mode );
		
		if ( merge == NULL )
		{
			return;
		}
		
		const int pixel_size = 4;
		
		size_t gutter = stride - width * pixel_size;
		
		byte const* a = (byte const*) left;
		byte*       b = (byte*)       right;
		
		for ( unsigned y = 0;  y < height;  ++y )
		{
			for ( unsigned x = 0;  x < width;  ++x )
			{
				merge( a, b );
				
				a += pixel_size;
				b += pixel_size;
			}
			
			a += gutter;
			b += gutter;
		}
	}
	
}

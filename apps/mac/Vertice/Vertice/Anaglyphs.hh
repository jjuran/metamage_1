/*
	Vertice/Anaglyphs.hh
	--------------------
*/

#ifndef VERTICE_ANAGLYPHS_HH
#define VERTICE_ANAGLYPHS_HH

// Standard C
#include <stddef.h>


namespace Vertice
{
	
	enum anaglyph_mode
	{
		kNoAnaglyph,
		kTrueAnaglyph,
		kGrayAnaglyph,
		kColorAnaglyph,
		kHalfColorAnaglyph,
		kOptimizedAnaglyph,
	};
	
	void merge_anaglyph( anaglyph_mode  mode,
	                     void const*    left,
	                     void*          right,
	                     size_t         height,
	                     size_t         width,
	                     size_t         stride );
	
}

#endif

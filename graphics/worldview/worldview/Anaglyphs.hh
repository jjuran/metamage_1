/*
	worldview/Anaglyphs.hh
	----------------------
*/

#ifndef WORLDVIEW_ANAGLYPHS_HH
#define WORLDVIEW_ANAGLYPHS_HH

// Standard C
#include <stddef.h>


namespace worldview
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

/*
	BlueBoxed.hh
	------------
*/

#ifndef MACFEATURES_BLUEBOXED_HH
#define MACFEATURES_BLUEBOXED_HH


namespace MacFeatures
{
	
	bool Is_BlueBoxed();
	
	
#ifdef __MACH__
	
	inline bool Is_BlueBoxed()
	{
		// Mach-O is definitely not in Classic.  But everything else could be.
		return false;
	}
	
#endif
	
}

#endif


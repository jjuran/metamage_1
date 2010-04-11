/*	==================
 *	ExtractIncludes.hh
 *	==================
 */

#ifndef ALINE_EXTRACTINCLUDES_HH
#define ALINE_EXTRACTINCLUDES_HH


namespace tool
{
	
	struct IncludesCache;
	
	void ExtractIncludes( IncludesCache& result, const char* pathname );
	
}

#endif


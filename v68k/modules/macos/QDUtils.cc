/*
	QDUtils.cc
	----------
*/

#include "QDUtils.hh"

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// macos
#include "QDGlobals.hh"


pascal short Random_patch()
{
	QDGlobals& qd = get_QDGlobals();
	
	qd.randSeed = qd.randSeed * 16807 % 0x7FFFFFFF;
	
	return qd.randSeed;
}

pascal PatHandle GetPattern_patch( short id )
{
	return (PatHandle) GetResource( 'PAT ', id );
}

pascal CursHandle GetCursor_patch( short id )
{
	return (CursHandle) GetResource( 'CURS', id );
}

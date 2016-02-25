/*
	QDUtils.cc
	----------
*/

#include "QDUtils.hh"

// macos
#include "QDGlobals.hh"


pascal short Random_patch()
{
	QDGlobals& qd = get_QDGlobals();
	
	qd.randSeed = qd.randSeed * 16807 % 0x7FFFFFFF;
	
	return qd.randSeed;
}

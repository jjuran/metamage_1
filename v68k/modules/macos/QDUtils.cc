/*
	QDUtils.cc
	----------
*/

#include "QDUtils.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// macos
#include "QDGlobals.hh"


static inline
QDGlobals& get_QDGlobals()
{
	GrafPtr* const thePort_addr = get_addrof_thePort();
	
	return *(QDGlobals*) ((char*) thePort_addr - offsetof(QDGlobals, thePort));
}

pascal short Random_patch()
{
	QDGlobals& qd = get_QDGlobals();
	
	qd.randSeed = qd.randSeed * 16807 % 0x7FFFFFFF;
	
	return qd.randSeed;
}

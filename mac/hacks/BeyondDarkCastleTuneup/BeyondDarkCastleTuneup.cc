/*
	BeyondDarkCastleTuneup.cc
	-------------------------
	
	Beyond Dark Castle Tune-up INIT for Advanced Mac Substitute
	
	Copyright 2026, Joshua Juran.  All rights reserved.
	
	This is an application hot patch for Beyond Dark Castle.  Note
	that the Beyond Dark Castle application is a shim that launches
	"BDC Data A", which is the real application.  Also note that only
	the Beyond Dark Castle file has the 'dc2j' owner resource, so
	we identify BDC Data A by the presence of 'GDSK' id=0 instead.
	
*/

#define IDENTIFYING_RESTYPE  'GDSK'

#define ENVCHECK_CODE_RESID  4
#define SPINLOOP_CODE_RESID  3

#define ENVCHECK_JSR  0x4EBAFE2E

#define OFFSET_TO_ENVCHECK_JSR  0x02ac
#define OFFSET_TO_SPINLOOP_BNE  0x084a

// DarkCastleTuneupHelper
#include "DarkCastleTuneupHelper.cc.hh"

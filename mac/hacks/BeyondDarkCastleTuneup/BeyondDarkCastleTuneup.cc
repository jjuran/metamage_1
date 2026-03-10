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

#define SAVEGAME_CODE_RESID  12

#define ENVCHECK_CODE_RESID  4
#define SPINLOOP_CODE_RESID  3
#define BLITTING_CODE_RESID  3
#define FIRESIDE_CODE_RESID  5

#define OFFSET_TO_BLITTER_READ  0x06b2
#define OFFSET_TO_SPINLOOP_BNE  0x084a
#define OFFSET_TO_PAUSING_TRAP  0x32d0

// CastleTuneup-template
#include "CastleTuneup.cc.hh"

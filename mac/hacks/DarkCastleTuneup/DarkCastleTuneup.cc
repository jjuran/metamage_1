/*
	DarkCastleTuneup.cc
	-------------------
	
	Dark Castle Tune-up INIT for Advanced Mac Substitute
	
	Copyright 2020-2026, Joshua Juran.  All rights reserved.
	
	This is a set of application hot patches for Dark Castle.
	Note that the Dark Castle application is actually a shim
	that launches "Data A", which is the real application.
	Also note that both contain the 'DCAS' owner resource,
	although only Data A has 'CODE' id=3 or id=4.
	
*/

#define IDENTIFYING_RESTYPE  'DCAS'

#define ENVCHECK_CODE_RESID  3
#define SPINLOOP_CODE_RESID  4

#define ENVCHECK_JSR  0x4EAD0072

#define OFFSET_TO_ENVCHECK_JSR  0x0496
#define OFFSET_TO_SPINLOOP_BNE  0x074a

// CastleTuneup-template
#include "CastleTuneup.cc.hh"

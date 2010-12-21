// Nitrogen/ASRegistry.hh
// ----------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_ASREGISTRY_HH
#define NITROGEN_ASREGISTRY_HH

// Nitrogen
#ifndef MAC_APPLEEVENTS_FUNCTIONS_AEDISPOSETOKEN_HH
#include "Mac/AppleEvents/Functions/AEDisposeToken.hh"
#endif


namespace Nitrogen
{
	
	// kUpdateAEUT .. kCleanUpAEUT
	
	// kASPrepositionalSubroutine
	
	// kASStartLogEvent .. kASStopLogEvent
	// kASAdd .. kASPower
	// kASComesBefore .. kASConcatenate
	// kASAnd .. keyASArg
	// kASErrorEventCode .. typeUserRecordFields
	
	// kDialectBundleResType
	
	// kAETrue .. kAEFalse
	
	// kASCurrentApplication
	
	// pASIt .. pASTopLevelScript
	// kAECase .. enumConsiderations
	// kAECaseConsiderMask .. enumConsidsAndIgnores
	// cCoercion .. cStorage
	// pASWeekday .. cDecember
	// cWeekday .. kASMagicEndTellEvent
	
	// Do we ever need the missing value outside an object accessor?
	nucleus::owned< Mac::AEDesc_Token > MissingValue();
	
}

#endif


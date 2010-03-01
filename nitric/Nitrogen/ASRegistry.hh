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

// Mac OS
#ifndef __ASREGISTRY__
#include <ASRegistry.h>
#endif

// Nitrogen
#ifndef NITROGEN_AEOBJECTS_HH
#include "Nitrogen/AEObjects.hh"
#endif


namespace Nitrogen
{
	
	static const AEEventClass kASAppleScriptSuite  = AEEventClass( ::kASAppleScriptSuite  );
	static const AEEventClass kASScriptEditorSuite = AEEventClass( ::kASScriptEditorSuite );
	static const AEEventClass kASTypeNamesSuite    = AEEventClass( ::kASTypeNamesSuite    );
	
	// kGetAETE .. kASLaunchEvent
	
	// kASSubroutineEvent
	
	// kASPrepositionalSubroutine
	
	// kASStartLogEvent .. kASCommentEvent
	// kASAdd .. kASContains
	// kASAnd .. keyASArg
	// kASErrorEventCode .. typeUserRecordFields
	
	// kDialectBundleResType
	
	
	static const AEEnumeration enumBooleanValues = AEEnumeration( ::enumBooleanValues );
	
	// kAETrue .. kAEFalse
	
	static const AEEnumeration enumMiscValues = AEEnumeration( ::enumMiscValues );
	
	// kASCurrentApplication
	
	static const AEKeyForm formUserPropertyID = AEKeyForm( ::formUserPropertyID );
	
	// pASIt .. pASTopLevelScript
	// kAECase .. enumConsiderations
	// kAECaseConsiderMask .. enumConsidsAndIgnores
	// cCoercion .. cStorage
	// pASWeekday .. cDecember
	// cWeekday .. kASMagicEndTellEvent
	
	// Do we ever need the missing value outside an object accessor?
	inline nucleus::owned< AEDesc_Token > MissingValue()
	{
		return AECreateDesc< typeType, AEDesc_Token >( DescType( ::cMissingValue ) );
	}
	
}

#endif


/*	============
 *	ASRegistry.h
 *	============
 */

#ifndef NITROGEN_ASREGISTRY_H
#define NITROGEN_ASREGISTRY_H

// Mac OS
#ifndef __ASREGISTRY__
#include <ASRegistry.h>
#endif

// Nitrogen
#ifndef NITROGEN_AEOBJECTS_H
#include "Nitrogen/AEObjects.h"
#endif


namespace Nitrogen
{
	
	static const AEKeyword keyAETarget    = AEKeyword( ::keyAETarget    );
	static const AEKeyword keySubjectAttr = AEKeyword( ::keySubjectAttr );
	static const AEKeyword keyASReturning = AEKeyword( ::keyASReturning );
	
	static const AEEventClass kASAppleScriptSuite  = AEEventClass( ::kASAppleScriptSuite  );
	static const AEEventClass kASScriptEditorSuite = AEEventClass( ::kASScriptEditorSuite );
	static const AEEventClass kASTypeNamesSuite    = AEEventClass( ::kASTypeNamesSuite    );
	
	static const DescType typeAETE = DescType( ::typeAETE );
	static const DescType typeAEUT = DescType( ::typeAEUT );
	
	// kGetAETE .. kASLaunchEvent
	
	static const AEKeyword keyScszResource = AEKeyword( ::keyScszResource );
	static const DescType typeScszResource = DescType( ::typeScszResource );
	
	// kASSubroutineEvent
	
	static const AEKeyword keyASSubroutineName = AEKeyword( ::keyASSubroutineName );
	
	// kASPrepositionalSubroutine
	
	static const AEKeyword keyASPositionalArgs = AEKeyword( ::keyASPositionalArgs );
	
	static const AEKeyword keyAppHandledCoercion = AEKeyword( ::keyAppHandledCoercion );
	
	// kASStartLogEvent .. kASCommentEvent
	// kASAdd .. kASContains
	// kASAnd .. keyASArg
	// kASErrorEventCode .. typeUserRecordFields
	// keyASPrepositionAt .. keyASPrepositionUntil
	
	// kDialectBundleResType
	
	static const AEObjectClass cConstant               = AEObjectClass( ::cConstant               );
	static const AEObjectClass cClassIdentifier        = AEObjectClass( ::cClassIdentifier        );
	static const AEObjectClass cObjectBeingExamined    = AEObjectClass( ::cObjectBeingExamined    );
	static const AEObjectClass cList                   = AEObjectClass( ::cList                   );
	static const AEObjectClass cSmallReal              = AEObjectClass( ::cSmallReal              );
	static const AEObjectClass cReal                   = AEObjectClass( ::cReal                   );
	static const AEObjectClass cRecord                 = AEObjectClass( ::cRecord                 );
	static const AEObjectClass cReference              = AEObjectClass( ::cReference              );
	static const AEObjectClass cUndefined              = AEObjectClass( ::cUndefined              );
	static const AEObjectClass cMissingValue           = AEObjectClass( ::cMissingValue           );
	static const AEObjectClass cSymbol                 = AEObjectClass( ::cSymbol                 );
	static const AEObjectClass cLinkedList             = AEObjectClass( ::cLinkedList             );
	static const AEObjectClass cVector                 = AEObjectClass( ::cVector                 );
	static const AEObjectClass cEventIdentifier        = AEObjectClass( ::cEventIdentifier        );
	static const AEObjectClass cKeyIdentifier          = AEObjectClass( ::cKeyIdentifier          );
	static const AEObjectClass cUserIdentifier         = AEObjectClass( ::cUserIdentifier         );
	static const AEObjectClass cPreposition            = AEObjectClass( ::cPreposition            );
	static const AEObjectClass cKeyForm                = AEObjectClass( ::cKeyForm                );
	static const AEObjectClass cScript                 = AEObjectClass( ::cScript                 );
	static const AEObjectClass cHandler                = AEObjectClass( ::cHandler                );
	static const AEObjectClass cProcedure              = AEObjectClass( ::cProcedure              );
	static const AEObjectClass cHandleBreakpoint       = AEObjectClass( ::cHandleBreakpoint       );
	static const AEObjectClass cClosure                = AEObjectClass( ::cClosure                );
	static const AEObjectClass cRawData                = AEObjectClass( ::cRawData                );
	static const AEObjectClass cStringClass            = AEObjectClass( ::cStringClass            );
	static const AEObjectClass cNumber                 = AEObjectClass( ::cNumber                 );
	static const AEObjectClass cListElement            = AEObjectClass( ::cListElement            );
	static const AEObjectClass cListOrRecord           = AEObjectClass( ::cListOrRecord           );
	static const AEObjectClass cListOrString           = AEObjectClass( ::cListOrString           );
	static const AEObjectClass cListRecordOrString     = AEObjectClass( ::cListRecordOrString     );
	static const AEObjectClass cNumberOrString         = AEObjectClass( ::cNumberOrString         );
	static const AEObjectClass cNumberOrDateTime       = AEObjectClass( ::cNumberOrDateTime       );
	static const AEObjectClass cNumberDateTimeOrString = AEObjectClass( ::cNumberDateTimeOrString );
	static const AEObjectClass cAliasOrString          = AEObjectClass( ::cAliasOrString          );
	static const AEObjectClass cSeconds                = AEObjectClass( ::cSeconds                );
	
	static const DescType typeSound = DescType( ::typeSound );
	
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
	inline Nucleus::Owned< AEDescList, AETokenDisposer > MissingValue()
	{
		return AECreateToken< typeType >( DescType( ::cMissingValue ) );
	}
	
}

#endif


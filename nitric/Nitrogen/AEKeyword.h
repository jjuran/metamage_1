// AEKeyword.h

#ifndef NITROGEN_AEKEYWORD_H
#define NITROGEN_AEKEYWORD_H

#ifndef NUCLEUS_ENUMERATION_H
#include "Nucleus/Enumeration.h"
#endif

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __AEDATAMODEL__
#include FRAMEWORK_HEADER(AE,AEDataModel.h)
#endif
#ifndef __AEOBJECTS__
#include FRAMEWORK_HEADER(AE,AEObjects.h)
#endif
#ifndef __AEREGISTRY__
#include FRAMEWORK_HEADER(AE,AERegistry.h)
#endif
#ifndef __ASREGISTRY__
#include <ASRegistry.h>
#endif


namespace Nitrogen
{
	
	enum AEKeyword
	{
		
		// AEDataModel
		// -----------
		
		keyTransactionIDAttr   = ::keyTransactionIDAttr,
		keyReturnIDAttr        = ::keyReturnIDAttr,
		keyEventClassAttr      = ::keyEventClassAttr,
		keyEventIDAttr         = ::keyEventIDAttr,
		keyAddressAttr         = ::keyAddressAttr,
		keyOptionalKeywordAttr = ::keyOptionalKeywordAttr,
		keyTimeoutAttr         = ::keyTimeoutAttr,
		keyInteractLevelAttr   = ::keyInteractLevelAttr,
		keyEventSourceAttr     = ::keyEventSourceAttr,
		keyMissedKeywordAttr   = ::keyMissedKeywordAttr,
		keyOriginalAddressAttr = ::keyOriginalAddressAttr,
		keyAcceptTimeoutAttr   = ::keyAcceptTimeoutAttr,
		
		// AEObjects
		// ---------
		
		keyAECompOperator    = ::keyAECompOperator,
		keyAELogicalTerms    = ::keyAELogicalTerms,
		keyAELogicalOperator = ::keyAELogicalOperator,
		keyAEObject1         = ::keyAEObject1,
		keyAEObject2         = ::keyAEObject2,
		
		keyAEDesiredClass    = ::keyAEDesiredClass,
		keyAEContainer       = ::keyAEContainer,
		keyAEKeyForm         = ::keyAEKeyForm,
		keyAEKeyData         = ::keyAEKeyData,
		
		keyAERangeStart      = ::keyAERangeStart,
		keyAERangeStop       = ::keyAERangeStop,
		
		keyDisposeTokenProc  = ::keyDisposeTokenProc,
		keyAECompareProc     = ::keyAECompareProc,
		keyAECountProc       = ::keyAECountProc,
		keyAEMarkTokenProc   = ::keyAEMarkTokenProc,
		keyAEMarkProc        = ::keyAEMarkProc,
		keyAEAdjustMarksProc = ::keyAEAdjustMarksProc,
		keyAEGetErrDescProc  = ::keyAEGetErrDescProc,
		
		// AERegistry
		// ----------
		
		keyAEAngle           = ::keyAEAngle,
		keyAEArcAngle        = ::keyAEArcAngle,
		
		keyAEBaseAddr        = ::keyAEBaseAddr,
		keyAEBestType        = ::keyAEBestType,
		keyAEBgndColor       = ::keyAEBgndColor,
		keyAEBgndPattern     = ::keyAEBgndPattern,
		keyAEBounds          = ::keyAEBounds,
		keyAECellList        = ::keyAECellList,
		keyAEClassID         = ::keyAEClassID,
		keyAEColor           = ::keyAEColor,
		keyAEColorTable      = ::keyAEColorTable,
		keyAECurveHeight     = ::keyAECurveHeight,
		keyAECurveWidth      = ::keyAECurveWidth,
		keyAEDashStyle       = ::keyAEDashStyle,
		keyAEData            = ::keyAEData,
		keyAEDefaultType     = ::keyAEDefaultType,
		keyAEDefinitionRect  = ::keyAEDefinitionRect,
		keyAEDescType        = ::keyAEDescType,
		keyAEDestination     = ::keyAEDestination,
		keyAEDoAntiAlias     = ::keyAEDoAntiAlias,
		keyAEDoDithered      = ::keyAEDoDithered,
		keyAEDoRotate        = ::keyAEDoRotate,
		
		keyAEDoScale         = ::keyAEDoScale,
		keyAEDoTranslate     = ::keyAEDoTranslate,
		keyAEEditionFileLoc  = ::keyAEEditionFileLoc,
		keyAEElements        = ::keyAEElements,
		keyAEEndPoint        = ::keyAEEndPoint,
		keyAEEventClass      = ::keyAEEventClass,
		keyAEEventID         = ::keyAEEventID,
		keyAEFile            = ::keyAEFile,
		keyAEFileType        = ::keyAEFileType,
		keyAEFillColor       = ::keyAEFillColor,
		keyAEFillPattern     = ::keyAEFillPattern,
		keyAEFlipHorizontal  = ::keyAEFlipHorizontal,
		keyAEFlipVertical    = ::keyAEFlipVertical,
		keyAEFont            = ::keyAEFont,
		keyAEFormula         = ::keyAEFormula,
		keyAEGraphicObjects  = ::keyAEGraphicObjects,
		keyAEID              = ::keyAEID,
		keyAEImageQuality    = ::keyAEImageQuality,
		keyAEInsertHere      = ::keyAEInsertHere,
		keyAEKeyForms        = ::keyAEKeyForms,
		
		keyAEKeyword         = ::keyAEKeyword,
		keyAELevel           = ::keyAELevel,
		keyAELineArrow       = ::keyAELineArrow,
		keyAEName            = ::keyAEName,
		keyAENewElementLoc   = ::keyAENewElementLoc,
		keyAEObject          = ::keyAEObject,
		keyAEObjectClass     = ::keyAEObjectClass,
		keyAEOffStyles       = ::keyAEOffStyles,
		keyAEOnStyles        = ::keyAEOnStyles,
		keyAEParameters      = ::keyAEParameters,
		keyAEParamFlags      = ::keyAEParamFlags,
		keyAEPenColor        = ::keyAEPenColor,
		keyAEPenPattern      = ::keyAEPenPattern,
		keyAEPenWidth        = ::keyAEPenWidth,
		keyAEPixelDepth      = ::keyAEPixelDepth,
		keyAEPixMapMinus     = ::keyAEPixMapMinus,
		keyAEPMTable         = ::keyAEPMTable,
		keyAEPointList       = ::keyAEPointList,
		keyAEPointSize       = ::keyAEPointSize,
		keyAEPosition        = ::keyAEPosition,
		
		keyAEPropData        = ::keyAEPropData,
		keyAEProperties      = ::keyAEProperties,
		keyAEProperty        = ::keyAEProperty,
		keyAEPropFlags       = ::keyAEPropFlags,
		keyAEPropID          = ::keyAEPropID,
		keyAEProtection      = ::keyAEProtection,
		keyAERenderAs        = ::keyAERenderAs,
		keyAERequestedType   = ::keyAERequestedType,
		keyAEResult          = ::keyAEResult,
		keyAEResultInfo      = ::keyAEResultInfo,
		keyAERotation        = ::keyAERotation,
		keyAERotPoint        = ::keyAERotPoint,
		keyAERowList         = ::keyAERowList,
		keyAESaveOptions     = ::keyAESaveOptions,
		keyAEScale           = ::keyAEScale,
		keyAEScriptTag       = ::keyAEScriptTag,
		keyAEShowWhere       = ::keyAEShowWhere,
		keyAEStartAngle      = ::keyAEStartAngle,
		keyAEStartPoint      = ::keyAEStartPoint,
		keyAEStyles          = ::keyAEStyles,
		
		keyAESuiteID         = ::keyAESuiteID,
		keyAEText            = ::keyAEText,
		keyAETextColor       = ::keyAETextColor,
		keyAETextFont        = ::keyAETextFont,
		keyAETextPointSize   = ::keyAETextPointSize,
		keyAETextStyles      = ::keyAETextStyles,
		keyAETextLineHeight  = ::keyAETextLineHeight,
		keyAETextLineAscent  = ::keyAETextLineAscent,
		keyAETheText         = ::keyAETheText,
		keyAETransferMode    = ::keyAETransferMode,
		keyAETranslation     = ::keyAETranslation,
		keyAETryAsStructGraf = ::keyAETryAsStructGraf,
		keyAEUniformStyles   = ::keyAEUniformStyles,
		keyAEUpdateOn        = ::keyAEUpdateOn,
		keyAEUserTerm        = ::keyAEUserTerm,
		keyAEWindow          = ::keyAEWindow,
		keyAEWritingCode     = ::keyAEWritingCode,
		
		keyMiscellaneous     = ::keyMiscellaneous,
		keySelection         = ::keySelection,
		keyWindow            = ::keyWindow,
		
		keyWhen              = ::keyWhen,
		keyWhere             = ::keyWhere,
		keyModifiers         = ::keyModifiers,
		keyKey               = ::keyKey,
		keyKeyCode           = ::keyKeyCode,
		keyKeyboard          = ::keyKeyboard,
		keyDriveNumber       = ::keyDriveNumber,
		keyErrorCode         = ::keyErrorCode,
		keyHighLevelClass    = ::keyHighLevelClass,
		keyHighLevelID       = ::keyHighLevelID,
		
		// ASRegistry
		// ----------
		
		keyAETarget    = ::keyAETarget,
		keySubjectAttr = ::keySubjectAttr,
		keyASReturning = ::keyASReturning,
		
		keyScszResource = ::keyScszResource,
		
		keyASSubroutineName = ::keyASSubroutineName,
		
		keyASPositionalArgs = ::keyASPositionalArgs,
		
		keyAppHandledCoercion = ::keyAppHandledCoercion,
		
		// keyASPrepositionAt .. keyASPrepositionUntil
		
		kAEKeyword_Max = Nucleus::Enumeration_Traits< ::AEKeyword >::max
	};
	
}

#endif


/*
	Mac/AppleEvents/Types/AEPropertyID.hh
	-------------------------------------
*/

#ifndef MAC_APPLEEVENTS_TYPES_AEPROPERTYID_HH
#define MAC_APPLEEVENTS_TYPES_AEPROPERTYID_HH

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __AEREGISTRY__
#include <AERegistry.h>
#endif

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif


namespace Mac
{
	
	// Christopher Nebel personally gave his blessing to this name at WWDC 2006
	
	enum AEPropertyID
	{
		
		pAll = ::kAEAll,
		
		// AERegistry
		// ----------
		
		pArcAngle           = ::pArcAngle,
		pBackgroundColor    = ::pBackgroundColor,
		pBackgroundPattern  = ::pBackgroundPattern,
		pBestType           = ::pBestType,
		pBounds             = ::pBounds,
		pClass              = ::pClass,
		pClipboard          = ::pClipboard,
		pColor              = ::pColor,
		pColorTable         = ::pColorTable,
		pContents           = ::pContents,
		pCornerCurveHeight  = ::pCornerCurveHeight,
		pCornerCurveWidth   = ::pCornerCurveWidth,
		pDashStyle          = ::pDashStyle,
		pDefaultType        = ::pDefaultType,
		pDefinitionRect     = ::pDefinitionRect,
		pEnabled            = ::pEnabled,
		pEndPoint           = ::pEndPoint,
		pFillColor          = ::pFillColor,
		pFillPattern        = ::pFillPattern,
		pFont               = ::pFont,
		pFormula            = ::pFormula,
		pGraphicObjects     = ::pGraphicObjects,
		pHasCloseBox        = ::pHasCloseBox,
		pHasTitleBar        = ::pHasTitleBar,
		pID                 = ::pID,
		pIndex              = ::pIndex,
		pInsertionLoc       = ::pInsertionLoc,
		pIsFloating         = ::pIsFloating,
		pIsFrontProcess     = ::pIsFrontProcess,
		pIsModal            = ::pIsModal,
		pIsModified         = ::pIsModified,
		pIsResizable        = ::pIsResizable,
		pIsStationeryPad    = ::pIsStationeryPad,
		pIsZoomable         = ::pIsZoomable,
		pIsZoomed           = ::pIsZoomed,
		pItemNumber         = ::pItemNumber,
		pJustification      = ::pJustification,
		pLineArrow          = ::pLineArrow,
		pMenuID             = ::pMenuID,
		pName               = ::pName,
		pNewElementLoc      = ::pNewElementLoc,
		pPenColor           = ::pPenColor,
		pPenPattern         = ::pPenPattern,
		pPenWidth           = ::pPenWidth,
		pPixelDepth         = ::pPixelDepth,
		pPointList          = ::pPointList,
		pPointSize          = ::pPointSize,
		pProtection         = ::pProtection,
		pRotation           = ::pRotation,
		pScale              = ::pScale,
		pScript             = ::pScript,
		pScriptTag          = ::pScriptTag,
		pSelected           = ::pSelected,
		pSelection          = ::pSelection,
		pStartAngle         = ::pStartAngle,
		pStartPoint         = ::pStartPoint,
		pTextColor          = ::pTextColor,
		pTextFont           = ::pTextFont,
		pTextItemDelimiters = ::pTextItemDelimiters,
		pTextPointSize      = ::pTextPointSize,
		pTextStyles         = ::pTextStyles,
		pTransferMode       = ::pTransferMode,
		pTranslation        = ::pTranslation,
		pUniformStyles      = ::pUniformStyles,
		pUpdateOn           = ::pUpdateOn,
		pUserSelection      = ::pUserSelection,
		pVersion            = ::pVersion,
		pVisible            = ::pVisible,
		
		kAEPropertyID_Max = nucleus::enumeration_traits< ::DescType >::max
	};
	
}

#endif


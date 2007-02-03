// AEPropertyID.h

#ifndef NITROGEN_AEPROPERTYID_H
#define NITROGEN_AEPROPERTYID_H

#ifndef NUCLEUS_ENUMERATION_H
#include "Nucleus/Enumeration.h"
#endif

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __AEREGISTRY__
#include FRAMEWORK_HEADER(AE,AERegistry.h)
#endif
#ifndef __ASREGISTRY__
#include <ASRegistry.h>
#endif


namespace Nitrogen
{
	
	enum AEPropertyID
	{
		
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
		
		// ASRegistry
		// ----------
		
		kAEPropertyID_Max = Nucleus::Enumeration_Traits< ::DescType >::max
	};
	
}

#endif

